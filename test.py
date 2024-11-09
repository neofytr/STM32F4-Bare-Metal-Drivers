import serial
import time

# Serial port configuration
SERIAL_PATH = "/dev/ttyACM0"  # Adjust this path if necessary
BAUD_RATE = 115200  # Match this to your device's baud rate

# Predefined 18 bytes to send
PREDEFINED_BYTES = b"\x07\x41\x42\x43\x44\x45\x46\x47\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x39"

def write_serial():
    # Open the serial port
    with serial.Serial(SERIAL_PATH, BAUD_RATE, timeout=1) as ser:
        print(f"Writing {len(PREDEFINED_BYTES)} bytes to {SERIAL_PATH}...")
        
        # Send the predefined 18 bytes
        ser.write(PREDEFINED_BYTES)
        print(f"Sent: {PREDEFINED_BYTES.hex()}")

def read_serial():
    # Open the serial port
    with serial.Serial(SERIAL_PATH, BAUD_RATE, timeout=1) as ser:
        print(f"Reading raw bytes from {SERIAL_PATH}... Press Ctrl+C to stop.")
        
        try:
            while True:
                if ser.in_waiting > 0:
                    # Read bytes from the serial port
                    data = ser.read(ser.in_waiting)
                    print(f"Received: {data.hex()}")  # Print the raw bytes
                time.sleep(0.1)  # Adjust the sleep as needed
        except KeyboardInterrupt:
            print("\nStopped reading.")
        except serial.SerialException as e:
            print(f"Serial error: {e}")

if __name__ == "__main__":
    write_serial()  # First, write the predefined bytes to the serial
    # time.sleep(1)   # Wait for a second to ensure data is sent
    read_serial()   # Then, read incoming data
