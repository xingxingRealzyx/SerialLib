#include "SerialPort.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <cstring>
#include <errno.h>

namespace Serial {

SerialPort::SerialPort() : fd_(-1) {
}

SerialPort::~SerialPort() {
    close();
}

bool SerialPort::open(const std::string& device) {
    if (isOpen()) {
        setError("Serial port is already open");
        return false;
    }
    
    device_ = device;
    
    // Open serial device file
    fd_ = ::open(device.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd_ == -1) {
        setError("Unable to open serial device: " + device + " - " + std::string(strerror(errno)));
        return false;
    }
    
    // Check if it's a terminal device
    if (!isatty(fd_)) {
        ::close(fd_);
        fd_ = -1;
        setError("Device is not a terminal device: " + device);
        return false;
    }
    
    // Get current serial port configuration
    struct termios options;
    if (tcgetattr(fd_, &options) != 0) {
        ::close(fd_);
        fd_ = -1;
        setError("Unable to get serial port attributes: " + std::string(strerror(errno)));
        return false;
    }
    
    // Set to blocking mode
    int flags = fcntl(fd_, F_GETFL);
    if (flags == -1) {
        ::close(fd_);
        fd_ = -1;
        setError("Unable to get file status flags: " + std::string(strerror(errno)));
        return false;
    }
    
    if (fcntl(fd_, F_SETFL, flags & ~O_NONBLOCK) == -1) {
        ::close(fd_);
        fd_ = -1;
        setError("Unable to set blocking mode: " + std::string(strerror(errno)));
        return false;
    }
    
    return true;
}

void SerialPort::close() {
    if (fd_ != -1) {
        ::close(fd_);
        fd_ = -1;
    }
    device_.clear();
    lastError_.clear();
}

bool SerialPort::isOpen() const {
    return fd_ != -1;
}

bool SerialPort::configure(BaudRate baudRate, DataBits dataBits, Parity parity, 
                          StopBits stopBits, FlowControl flowControl) {
    if (!isOpen()) {
        setError("Serial port is not open");
        return false;
    }
    
    struct termios options;
    
    // Get current attributes
    if (tcgetattr(fd_, &options) != 0) {
        setError("Unable to get serial port attributes: " + std::string(strerror(errno)));
        return false;
    }
    
    // Clear all flags
    options.c_cflag = 0;
    options.c_iflag = 0;
    options.c_oflag = 0;
    options.c_lflag = 0;
    
    // Set baud rate
    if (cfsetispeed(&options, static_cast<speed_t>(baudRate)) != 0 ||
        cfsetospeed(&options, static_cast<speed_t>(baudRate)) != 0) {
        setError("Unable to set baud rate: " + std::string(strerror(errno)));
        return false;
    }
    
    // Set data bits
    options.c_cflag |= static_cast<tcflag_t>(dataBits);
    
    // Set parity
    if (parity == Parity::EVEN) {
        options.c_cflag |= PARENB;
    } else if (parity == Parity::ODD) {
        options.c_cflag |= PARENB | PARODD;
    }
    
    // Set stop bits
    if (stopBits == StopBits::TWO) {
        options.c_cflag |= CSTOPB;
    }
    
    // Set flow control
    if (flowControl == FlowControl::HARDWARE) {
        options.c_cflag |= CRTSCTS;
    } else if (flowControl == FlowControl::SOFTWARE) {
        options.c_iflag |= IXON | IXOFF;
    }
    
    // Enable receiver, set local mode
    options.c_cflag |= CLOCAL | CREAD;
    
    // Set to raw mode
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;
    options.c_iflag &= ~(IXON | IXOFF | IXANY | ICRNL | INLCR | IGNCR);
    
    // Set default timeout and minimum character count
    options.c_cc[VTIME] = 10;  // 1 second timeout
    options.c_cc[VMIN] = 0;    // Non-blocking read
    
    return setTerminalAttributes(options);
}

int SerialPort::write(const void* data, size_t size) {
    if (!isOpen()) {
        setError("Serial port is not open");
        return -1;
    }
    
    ssize_t result = ::write(fd_, data, size);
    if (result == -1) {
        setError("Failed to write data: " + std::string(strerror(errno)));
        return -1;
    }
    
    return static_cast<int>(result);
}

int SerialPort::write(const std::string& data) {
    return write(data.c_str(), data.size());
}

int SerialPort::write(const void* data, size_t size, bool waitForCompletion) {
    int result = write(data, size);
    
    if (result > 0 && waitForCompletion) {
        if (!drain()) {
            // Still return the number of bytes written, but set error for drain failure
            setError("Data written but failed to wait for transmission completion: " + lastError_);
        }
    }
    
    return result;
}

int SerialPort::write(const std::string& data, bool waitForCompletion) {
    return write(data.c_str(), data.size(), waitForCompletion);
}

bool SerialPort::drain() {
    if (!isOpen()) {
        setError("Serial port is not open");
        return false;
    }
    
    // tcdrain() waits until all output written to the object referred by fd has been transmitted
    if (tcdrain(fd_) != 0) {
        setError("Failed to drain output buffer: " + std::string(strerror(errno)));
        return false;
    }
    
    return true;
}

int SerialPort::read(void* buffer, size_t size, int timeoutMs) {
    if (!isOpen()) {
        setError("Serial port is not open");
        return -1;
    }
    
    // Set timeout for this read operation
    if (!setReadTimeout(timeoutMs)) {
        return -1;
    }
    
    ssize_t result = ::read(fd_, buffer, size);
    if (result == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;  // Timeout or no data available
        }
        setError("Failed to read data: " + std::string(strerror(errno)));
        return -1;
    }
    
    return static_cast<int>(result);
}

std::string SerialPort::read(size_t maxBytes, int timeoutMs) {
    if (!isOpen()) {
        setError("Serial port is not open");
        return "";
    }
    
    std::string result;
    result.reserve(maxBytes);
    
    char buffer[1024];
    size_t totalRead = 0;
    
    while (totalRead < maxBytes) {
        size_t toRead = std::min(sizeof(buffer), maxBytes - totalRead);
        int bytesRead = read(buffer, toRead, timeoutMs);
        
        if (bytesRead < 0) {
            return result;  // Error
        } else if (bytesRead == 0) {
            break;  // Timeout or no more data
        }
        
        result.append(buffer, bytesRead);
        totalRead += bytesRead;
    }
    
    return result;
}

bool SerialPort::flush() {
    if (!isOpen()) {
        setError("Serial port is not open");
        return false;
    }
    
    if (tcflush(fd_, TCIOFLUSH) != 0) {
        setError("Failed to flush buffers: " + std::string(strerror(errno)));
        return false;
    }
    
    return true;
}

bool SerialPort::flushInput() {
    if (!isOpen()) {
        setError("Serial port is not open");
        return false;
    }
    
    if (tcflush(fd_, TCIFLUSH) != 0) {
        setError("Failed to flush input buffer: " + std::string(strerror(errno)));
        return false;
    }
    
    return true;
}

bool SerialPort::flushOutput() {
    if (!isOpen()) {
        setError("Serial port is not open");
        return false;
    }
    
    if (tcflush(fd_, TCOFLUSH) != 0) {
        setError("Failed to flush output buffer: " + std::string(strerror(errno)));
        return false;
    }
    
    return true;
}

int SerialPort::available() const {
    if (!isOpen()) {
        return -1;
    }
    
    int bytes;
    if (ioctl(fd_, FIONREAD, &bytes) == -1) {
        return -1;
    }
    
    return bytes;
}

std::string SerialPort::getLastError() const {
    return lastError_;
}

bool SerialPort::setTerminalAttributes(const struct termios& options) {
    if (tcsetattr(fd_, TCSANOW, &options) != 0) {
        setError("Unable to set serial port attributes: " + std::string(strerror(errno)));
        return false;
    }
    
    // Wait for settings to take effect
    usleep(100000);  // 100ms
    
    return true;
}

bool SerialPort::setReadTimeout(int timeoutMs) {
    if (!isOpen()) {
        setError("Serial port is not open");
        return false;
    }
    
    struct termios options;
    if (tcgetattr(fd_, &options) != 0) {
        setError("Unable to get serial port attributes: " + std::string(strerror(errno)));
        return false;
    }
    
    // VTIME is in units of 1/10 second
    options.c_cc[VTIME] = timeoutMs / 100;
    if (options.c_cc[VTIME] == 0 && timeoutMs > 0) {
        options.c_cc[VTIME] = 1;
    }
    
    if (tcsetattr(fd_, TCSANOW, &options) != 0) {
        setError("Unable to set read timeout: " + std::string(strerror(errno)));
        return false;
    }
    
    return true;
}

void SerialPort::setError(const std::string& error) {
    lastError_ = error;
}

} // namespace Serial 