# Linux C++ Serial Communication Library

A simple and easy-to-use C++ serial communication library for Linux platforms, supporting common serial port operations and configurations. This library is designed to be **statically linked** for easy deployment without runtime dependencies.

## ✨ Features

- 🚀 Multiple baud rates (9600 - 921600)
- ⚙️ Configurable data bits, parity, stop bits, flow control
- ⏱️ **Flexible per-operation timeout**
- 🔒 **Guaranteed data transmission with drain functionality**
- 📦 **Static library for easy deployment**
- 🔧 **CMake integration support**
- 🛡️ Comprehensive error handling

## 🚀 Quick Start

### Build
```bash
git clone <repository-url>
cd Serial
./build.sh make          # or: ./build.sh cmake
```

### Basic Usage
```cpp
#include "SerialPort.h"
#include <iostream>

int main() {
    Serial::SerialPort serial;
    
    // Open and configure
    if (!serial.open("/dev/ttyUSB0")) {
        std::cerr << "Error: " << serial.getLastError() << std::endl;
        return 1;
    }
    
    serial.configure(Serial::BaudRate::BAUD_115200,
                    Serial::DataBits::BITS_8,
                    Serial::Parity::NONE,
                    Serial::StopBits::ONE,
                    Serial::FlowControl::NONE);
    
    // Send data
    serial.write("Hello, World!", true);  // Guaranteed transmission
    
    // Read data
    std::string response = serial.read(1024, 2000);  // 2s timeout
    std::cout << "Received: " << response << std::endl;
    
    return 0;
}
```

## 📦 Integration

### Method 1: System Installation (Recommended)
```bash
# Install
sudo ./build.sh install

# Use in your CMakeLists.txt
find_package(SerialLib REQUIRED)
target_link_libraries(your_app SerialLib::serial_static)
```

### Method 2: Add as Subdirectory
```bash
# Copy to your project
cp -r SerialLib /path/to/your/project/third_party/
```

```cmake
# In your CMakeLists.txt
add_subdirectory(third_party/SerialLib)
target_link_libraries(your_app serial_static)
```

### Method 3: Manual Linking
```bash
g++ -std=c++11 -Ipath/to/SerialLib/include your_app.cpp path/to/SerialLib/build/lib/libserial.a -o your_app
```

### Method 4: FetchContent (Automatic Download)
```cmake
include(FetchContent)
FetchContent_Declare(
    SerialLib
    GIT_REPOSITORY https://github.com/your-username/SerialLib.git
    GIT_TAG        v1.0.0
)
FetchContent_MakeAvailable(SerialLib)
target_link_libraries(your_app serial_static)
```

## 📚 API Reference

### Core Operations
```cpp
// Port management
bool open(const std::string& device);
void close();
bool isOpen() const;

// Configuration
bool configure(BaudRate, DataBits, Parity, StopBits, FlowControl);

// Data transmission
int write(const void* data, size_t size);                   // Fast write
int write(const void* data, size_t size, bool waitComplete); // Guaranteed write
bool drain();                                               // Wait for transmission

// Data reception
int read(void* buffer, size_t size, int timeoutMs = 1000);
std::string read(size_t maxBytes = 1024, int timeoutMs = 1000);

// Buffer management
bool flush();
bool flushInput();
bool flushOutput();
int available() const;

// Error handling
std::string getLastError() const;
```

### Supported Parameters
- **Baud Rates**: 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600
- **Data Bits**: 5, 6, 7, 8
- **Parity**: None, Odd, Even
- **Stop Bits**: 1, 2
- **Flow Control**: None, Hardware (RTS/CTS), Software (XON/XOFF)

## 🔧 Data Transmission Modes

```cpp
// Fast write (data goes to OS buffer)
serial.write("data");

// Guaranteed transmission (waits for completion)
serial.write("critical_data", true);

// Batch write with final guarantee
serial.write("chunk1");
serial.write("chunk2");
serial.drain();  // Ensure all data transmitted
```

## 🛠️ Build Options

```bash
./build.sh make      # Build with Makefile (default)
./build.sh cmake     # Build with CMake
./build.sh clean     # Clean build files
./build.sh install   # Install to system
./build.sh test      # Run examples
./build.sh help      # Show help
```

## 📋 Requirements

- Linux operating system
- GCC 4.8+ or Clang 3.4+
- CMake 3.10+ (optional)
- C++11 standard

## ⚠️ Common Issues

### 1. Permissions
```bash
sudo usermod -a -G dialout $USER
# Then logout and login again
```

### 2. Device Paths
- USB devices: `/dev/ttyUSB0`, `/dev/ttyUSB1`
- Arduino/CDC: `/dev/ttyACM0`, `/dev/ttyACM1`
- Traditional: `/dev/ttyS0`, `/dev/ttyS1`

### 3. Device Busy
Ensure no other programs are using the port

### 4. CMake Integration Issues
```cmake
# If library not found, set path
set(CMAKE_PREFIX_PATH "/path/to/SerialLib" ${CMAKE_PREFIX_PATH})
find_package(SerialLib REQUIRED)

# Or specify exact path
set(SerialLib_DIR "/path/to/SerialLib/lib/cmake/SerialLib")
```

## 📁 Project Structure

```
SerialLib/
├── README.md                   # English documentation
├── README_CN.md                # Chinese documentation
├── CMakeLists.txt              # CMake configuration
├── Makefile                    # Makefile build
├── build.sh                    # Build script
├── include/SerialPort.h        # Library header
├── src/SerialPort.cpp          # Implementation
├── examples/                   # Usage examples
└── cmake/                      # CMake configs
```

## 🔄 Uninstall

```bash
cd SerialLib/build
sudo make uninstall
# Or manually: sudo rm -f /usr/local/lib/libserial.a /usr/local/include/SerialPort.h
```

## 📄 License

MIT License - see LICENSE file for details.

## 🤝 Contributing

Issues and Pull Requests welcome!

---

**中文版本**: [README_ZH.md](README_ZH.md) 