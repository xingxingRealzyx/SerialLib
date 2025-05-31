#include "SerialPort.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "=== External Project Using SerialLib ===" << std::endl;
    std::cout << "This example shows how to use SerialLib in your own project" << std::endl;
    
    try {
        // Create serial port instance
        Serial::SerialPort serial;
        
        // Try to open a serial port (modify path as needed)
        std::string device = "/dev/ttyACM0";
        std::cout << "\nAttempting to open: " << device << std::endl;
        
        if (!serial.open(device)) {
            std::cout << "Failed to open serial port: " << serial.getLastError() << std::endl;
            std::cout << "This is expected if no serial device is connected." << std::endl;
            std::cout << "The important thing is that SerialLib linked successfully!" << std::endl;
            return 0;  // Not an error for this demo
        }
        
        std::cout << "✓ Serial port opened successfully!" << std::endl;
        
        // Configure serial port
        if (!serial.configure(
            Serial::BaudRate::BAUD_115200,
            Serial::DataBits::BITS_8,
            Serial::Parity::NONE,
            Serial::StopBits::ONE,
            Serial::FlowControl::NONE
        )) {
            std::cout << "Failed to configure serial port: " << serial.getLastError() << std::endl;
            return 1;
        }
        
        std::cout << "✓ Serial port configured (115200 8N1)" << std::endl;
        
        // Send test data
        std::string testData = "Hello from external project!";
        int written = serial.write(testData, true);  // Guaranteed transmission
        
        if (written > 0) {
            std::cout << "✓ Successfully sent " << written << " bytes" << std::endl;
        } else {
            std::cout << "Failed to send data: " << serial.getLastError() << std::endl;
        }
        
        // Try to read response
        std::string response = serial.read(256, 1000);  // 1 second timeout
        if (!response.empty()) {
            std::cout << "✓ Received response: " << response << std::endl;
        } else {
            std::cout << "No response received (timeout)" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\n=== SerialLib Integration Test Completed ===" << std::endl;
    std::cout << "If you see this message, SerialLib was successfully" << std::endl;
    std::cout << "linked and integrated into your external project!" << std::endl;
    
    return 0;
} 