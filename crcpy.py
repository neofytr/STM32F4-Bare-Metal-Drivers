# Re-defining the CRC function and calculating it for the specified byte sequence due to state reset.

# Given byte sequence
data = b'\x07\x41\x42\x43\x44\x45\x46\x47\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF'

# CRC-8 calculation function using polynomial 0x07 as in many standard protocols
def crc8(data: bytes, polynomial: int = 0x07) -> int:
    crc = 0
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 0x80:
                crc = ((crc << 1) ^ polynomial) & 0xFF
            else:
                crc = (crc << 1) & 0xFF
    return crc

# Calculate CRC for the data
crc_result = crc8(data)
print(crc_result)
