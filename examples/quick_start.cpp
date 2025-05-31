#include "SerialPort.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  Linux Serial Communication Library  " << std::endl;
    std::cout << "         Quick Start Example           " << std::endl;
    std::cout << "========================================" << std::endl;
    
    Serial::SerialPort serial;
    
    // Serial device path - please modify according to your device
    std::string device = "/dev/ttyACM0";
    
    std::cout << "\n1. Attempting to open serial port: " << device << std::endl;
    
    if (!serial.open(device)) {
        std::cerr << "   Error: Failed to open serial port" << std::endl;
        std::cerr << "   Details: " << serial.getLastError() << std::endl;
        std::cerr << "\n   Troubleshooting tips:" << std::endl;
        std::cerr << "   - Check if device exists: ls /dev/tty*" << std::endl;
        std::cerr << "   - Check permissions: sudo usermod -a -G dialout $USER" << std::endl;
        std::cerr << "   - Try different device path (ttyUSB0, ttyS0, etc.)" << std::endl;
        std::cerr << "   - Make sure no other program is using the port" << std::endl;
        return 1;
    }
    
    std::cout << "   Success: Serial port opened successfully" << std::endl;
    
    std::cout << "\n2. Configuring serial port parameters..." << std::endl;
    std::cout << "   Baud rate: 115200" << std::endl;
    std::cout << "   Data bits: 8" << std::endl;
    std::cout << "   Parity: None" << std::endl;
    std::cout << "   Stop bits: 1" << std::endl;
    std::cout << "   Flow control: None" << std::endl;
    
    if (!serial.configure(Serial::BaudRate::BAUD_115200,
                         Serial::DataBits::BITS_8,
                         Serial::Parity::NONE,
                         Serial::StopBits::ONE,
                         Serial::FlowControl::NONE)) {
        std::cerr << "   Error: Failed to configure serial port" << std::endl;
        std::cerr << "   Details: " << serial.getLastError() << std::endl;
        return 1;
    }
    
    std::cout << "   Success: Serial port configured successfully" << std::endl;
    
    std::cout << "\n3. Clearing buffers..." << std::endl;
    if (!serial.flush()) {
        std::cerr << "   Warning: Failed to flush buffers - " << serial.getLastError() << std::endl;
    } else {
        std::cout << "   Success: Buffers cleared" << std::endl;
    }
    
    std::cout << "\n4. Testing basic data transmission..." << std::endl;
    std::string testMessage = "Hello, Serial World!";
    std::cout << "   Sending: \"" << testMessage << "\"" << std::endl;
    
    int written = serial.write(testMessage);
    if (written > 0) {
        std::cout << "   Success: " << written << " bytes written to buffer" << std::endl;
    } else {
        std::cerr << "   Error: Failed to write data - " << serial.getLastError() << std::endl;
    }
    
    std::cout << "\n5. Testing guaranteed transmission..." << std::endl;
    std::string guaranteedMessage = "Guaranteed delivery!";
    std::cout << "   Sending with transmission guarantee: \"" << guaranteedMessage << "\"" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    int guaranteedWritten = serial.write(guaranteedMessage, true);  // Wait for completion
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    if (guaranteedWritten > 0) {
        std::cout << "   Success: " << guaranteedWritten << " bytes transmitted completely" << std::endl;
        std::cout << "   Transmission time: " << duration.count() << " microseconds" << std::endl;
    } else {
        std::cerr << "   Error: Failed to write data - " << serial.getLastError() << std::endl;
    }
    
    std::cout << "\n6. Testing manual drain operation..." << std::endl;
    std::string drainMessage = "Testing drain...";
    std::cout << "   Sending: \"" << drainMessage << "\"" << std::endl;
    
    written = serial.write(drainMessage);
    if (written > 0) {
        std::cout << "   " << written << " bytes written to buffer" << std::endl;
        std::cout << "   Manually waiting for transmission completion..." << std::endl;
        
        start = std::chrono::high_resolution_clock::now();
        if (serial.drain()) {
            end = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            std::cout << "   Success: All data transmitted (drain time: " << duration.count() << " microseconds)" << std::endl;
        } else {
            std::cerr << "   Warning: Failed to drain - " << serial.getLastError() << std::endl;
        }
    }
    
    // Wait a bit to allow response
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << "\n7. Checking for available data..." << std::endl;
    int available = serial.available();
    if (available > 0) {
        std::cout << "   " << available << " bytes available to read" << std::endl;
    } else {
        std::cout << "   No data available to read" << std::endl;
    }
    
    std::cout << "\n8. Testing data reception (with 2 second timeout)..." << std::endl;
    std::string received = serial.read(1024, 2000);  // 2 second timeout
    
    if (!received.empty()) {
        std::cout << "   Success: Received " << received.size() << " bytes" << std::endl;
        std::cout << "   Data: \"" << received << "\"" << std::endl;
    } else {
        std::cout << "   No data received within timeout period" << std::endl;
        std::cout << "   This is normal if no device is responding" << std::endl;
    }
    
    std::cout << "\n9. Final buffer flush..." << std::endl;
    if (serial.flush()) {
        std::cout << "   Success: Buffers flushed" << std::endl;
    } else {
        std::cerr << "   Warning: " << serial.getLastError() << std::endl;
    }
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "           Test Completed               " << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::cout << "\nLibrary features demonstrated:" << std::endl;
    std::cout << "✓ Serial port opening" << std::endl;
    std::cout << "✓ Parameter configuration" << std::endl;
    std::cout << "✓ Basic data transmission" << std::endl;
    std::cout << "✓ Guaranteed data transmission" << std::endl;
    std::cout << "✓ Manual transmission completion waiting" << std::endl;
    std::cout << "✓ Data reception with flexible timeout" << std::endl;
    std::cout << "✓ Buffer management" << std::endl;
    std::cout << "✓ Error handling" << std::endl;
    
    std::cout << "\nData transmission options:" << std::endl;
    std::cout << "- write(data): Fast write to buffer, may not immediately transmit" << std::endl;
    std::cout << "- write(data, true): Write and wait for complete transmission" << std::endl;
    std::cout << "- write(data) + drain(): Write then manually wait for transmission" << std::endl;
    std::cout << "- Use guaranteed transmission for critical data" << std::endl;
    std::cout << "- Use basic write for high-throughput scenarios" << std::endl;
    
    std::cout << "\nUsage notes:" << std::endl;
    std::cout << "- Each read operation can specify its own timeout" << std::endl;
    std::cout << "- Default timeout is 1000ms if not specified" << std::endl;
    std::cout << "- drain() ensures all data is physically transmitted" << std::endl;
    std::cout << "- Connect a loopback device or another serial device for better testing" << std::endl;
    std::cout << "- Check system logs: dmesg | grep tty" << std::endl;
    
    return 0;
} 