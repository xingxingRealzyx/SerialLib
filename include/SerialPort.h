#pragma once

#include <string>
#include <termios.h>

namespace Serial {

enum class BaudRate : speed_t {
    BAUD_9600 = B9600,
    BAUD_19200 = B19200,
    BAUD_38400 = B38400,
    BAUD_57600 = B57600,
    BAUD_115200 = B115200,
    BAUD_230400 = B230400,
    BAUD_460800 = B460800,
    BAUD_921600 = B921600
};

enum class DataBits : tcflag_t {
    BITS_5 = CS5,
    BITS_6 = CS6,
    BITS_7 = CS7,
    BITS_8 = CS8
};

enum class Parity : tcflag_t {
    NONE = 0,
    ODD = PARODD,
    EVEN = PARENB
};

enum class StopBits : tcflag_t {
    ONE = 0,
    TWO = CSTOPB
};

enum class FlowControl : tcflag_t {
    NONE = 0,
    HARDWARE = CRTSCTS,
    SOFTWARE = IXON | IXOFF
};

class SerialPort {
public:
    SerialPort();
    ~SerialPort();

    // Open serial port
    bool open(const std::string& device);
    
    // Close serial port
    void close();
    
    // Check if serial port is open
    bool isOpen() const;
    
    // Configure serial port parameters
    bool configure(BaudRate baudRate = BaudRate::BAUD_115200,
                  DataBits dataBits = DataBits::BITS_8,
                  Parity parity = Parity::NONE,
                  StopBits stopBits = StopBits::ONE,
                  FlowControl flowControl = FlowControl::NONE);
    
    // Write data
    int write(const void* data, size_t size);
    int write(const std::string& data);
    
    // Write data with option to wait for transmission completion
    int write(const void* data, size_t size, bool waitForCompletion);
    int write(const std::string& data, bool waitForCompletion);
    
    // Read data with timeout
    int read(void* buffer, size_t size, int timeoutMs = 1000);
    std::string read(size_t maxBytes = 1024, int timeoutMs = 1000);
    
    // Wait for all output data to be transmitted
    bool drain();
    
    // Flush buffers
    bool flush();
    bool flushInput();
    bool flushOutput();
    
    // Get number of available bytes to read
    int available() const;
    
    // Get last error message
    std::string getLastError() const;

private:
    int fd_;                    // File descriptor
    std::string device_;        // Device path
    std::string lastError_;     // Last error message
    
    // Helper functions
    bool setTerminalAttributes(const struct termios& options);
    bool setReadTimeout(int timeoutMs);
    void setError(const std::string& error);
};

} // namespace Serial
