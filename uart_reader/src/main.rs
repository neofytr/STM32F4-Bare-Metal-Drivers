use serialport::{SerialPort, SerialPortInfo, SerialPortType};
use std::io::{self, Read};
use std::time::Duration;

const PREDEFINED_BYTES: [u8; 18] = [
    0x07, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x39
];

fn main() -> io::Result<()> {
    // Port settings
    let port_name = "/dev/ttyACM0";  // Adjust as needed
    let baud_rate = 115_200;

    // Open and configure the port
    let mut port = serialport::new(port_name, baud_rate)
        .timeout(Duration::from_millis(10))
        .open()
        .expect("Failed to open serial port");

    println!("Serial port opened successfully");

    // Write the predefined bytes
    match port.write(&PREDEFINED_BYTES) {
        Ok(_) => println!("Sent: {:02X?}", PREDEFINED_BYTES),
        Err(e) => println!("Failed to write: {}", e),
    }

    // Buffer for reading
    let mut buffer = [0u8; 1024];
    println!("Reading data... Press Ctrl+C to exit");

    loop {
        match port.read(&mut buffer) {
            Ok(bytes_read) if bytes_read > 0 => {
                // Print received data in both hex and ASCII format
                print!("Hex: ");
                for byte in &buffer[..bytes_read] {
                    print!("{:02X} ", byte);
                }
                println!();

                print!("ASCII: ");
                for byte in &buffer[..bytes_read] {
                    if byte.is_ascii_control() {
                        print!("{}", *byte as char);
                    } else {
                        print!(".");
                    }
                }
                println!("\n");
            }
            Ok(_) => (), // No data received
            Err(ref e) if e.kind() == io::ErrorKind::TimedOut => (),
            Err(e) => eprintln!("Error: {}", e),
        }
    }
}