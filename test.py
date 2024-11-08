import serial
import time
from crc8 import crc8
import asyncio

# Constants for the packet protocol
PACKET_LENGTH_BYTES = 1
PACKET_DATA_BYTES = 16
PACKET_CRC_BYTES = 1
PACKET_CRC_INDEX = PACKET_LENGTH_BYTES + PACKET_DATA_BYTES
PACKET_LENGTH = PACKET_LENGTH_BYTES + PACKET_DATA_BYTES + PACKET_CRC_BYTES

PACKET_ACK_DATA0 = 0x15
PACKET_RETX_DATA0 = 0x19

# Details about the serial port connection
SERIAL_PATH = "/dev/ttyACM0"
BAUD_RATE = 115200

# Packet class
class Packet:
    def __init__(self, length, data, crc=None):
        self.length = length
        self.data = data
        self.padding_length = PACKET_DATA_BYTES - len(data)
        self.data += b'\xff' * self.padding_length
        if crc is None:
            self.crc = self.compute_crc()
        else:
            self.crc = crc

    def compute_crc(self):
        all_data = bytearray([self.length]) + self.data
        return crc8(all_data)

    def to_bytes(self):
        return bytes([self.length]) + self.data + bytes([self.crc])

    def is_single_byte_packet(self, byte):
        if self.length != 1:
            return False
        if self.data[0] != byte:
            return False
        for i in range(1, PACKET_DATA_BYTES):
            if self.data[i] != 0xff:
                return False
        return True

    def is_ack(self):
        return self.is_single_byte_packet(PACKET_ACK_DATA0)

    def is_retx(self):
        return self.is_single_byte_packet(PACKET_RETX_DATA0)

# Serial port instance
ser = serial.Serial(SERIAL_PATH, BAUD_RATE)

# Packet buffer
packets = []

last_packet = Packet(1, bytes([PACKET_ACK_DATA0]))

def write_packet(packet):
    ser.write(packet.to_bytes())
    global last_packet
    last_packet = packet

# Serial data buffer
rx_buffer = b''

def consume_from_buffer(n):
    global rx_buffer
    consumed = rx_buffer[:n]
    rx_buffer = rx_buffer[n:]
    return consumed

# Serial data callback
def on_serial_data(data):
    print(f"Received {len(data)} bytes through serial")
    global rx_buffer
    rx_buffer += data

    # Can we build a packet?
    while len(rx_buffer) >= PACKET_LENGTH:
        print("Building a packet")
        raw = consume_from_buffer(PACKET_LENGTH)
        packet = Packet(raw[0], raw[1:PACKET_DATA_BYTES+1], raw[PACKET_CRC_INDEX])
        computed_crc = packet.compute_crc()

        # Need retransmission?
        if packet.crc != computed_crc:
            print(f"CRC failed, computed 0x{computed_crc:02X}, got 0x{packet.crc:02X}")
            write_packet(Packet(1, bytes([PACKET_RETX_DATA0])))
            continue

        # Are we being asked to retransmit?
        if packet.is_retx():
            print("Retransmitting last packet")
            write_packet(last_packet)
            continue

        # If this is an ack, move on
        if packet.is_ack():
            print("It was an ack, nothing to do")
            continue

        # Otherwise write the packet in to the buffer, and send an ack
        print("Storing packet and ack'ing")
        packets.append(packet)
        write_packet(Packet(1, bytes([PACKET_ACK_DATA0])))

# Function to allow us to await a packet
async def wait_for_packet():
    while not packets:
        await asyncio.sleep(0.001)
    packet = packets.pop(0)
    return packet

async def main():
    # Open the serial port if it's not already open
    if not ser.is_open:
        ser.open()

    print("Waiting for packet...")
    packet = await wait_for_packet()
    print(packet)

    packet_to_send = Packet(4, bytes([5, 6, 7, 8]))
    packet_to_send.crc += 1
    write_packet(packet_to_send)

# Set up the serial port and start the main loop
ser.timeout = 0
ser.write_timeout = 0.1
ser.on_recv = on_serial_data
asyncio.run(main())