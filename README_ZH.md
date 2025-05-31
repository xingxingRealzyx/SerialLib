# Linux C++ 串口通信库

一个简单易用的Linux平台C++串口通信库，支持常见的串口操作和配置。该库设计为**静态链接**，便于部署且无运行时依赖。

## ✨ 特性

- 🚀 支持多种波特率 (9600 - 921600)
- ⚙️ 可配置数据位、校验位、停止位、流控制
- ⏱️ **灵活的单次操作超时设置**
- 🔒 **保证数据传输完成的drain功能**
- 📦 **静态库，便于部署**
- 🔧 **CMake集成支持**
- 🛡️ 完善的错误处理

## 🚀 快速开始

### 构建
```bash
git clone <仓库地址>
cd Serial
./build.sh make          # 或: ./build.sh cmake
```

### 基础使用
```cpp
#include "SerialPort.h"
#include <iostream>

int main() {
    Serial::SerialPort serial;
    
    // 打开并配置串口
    if (!serial.open("/dev/ttyUSB0")) {
        std::cerr << "错误: " << serial.getLastError() << std::endl;
        return 1;
    }
    
    serial.configure(Serial::BaudRate::BAUD_115200,
                    Serial::DataBits::BITS_8,
                    Serial::Parity::NONE,
                    Serial::StopBits::ONE,
                    Serial::FlowControl::NONE);
    
    // 发送数据
    serial.write("你好，串口！", true);  // 保证传输
    
    // 读取数据
    std::string response = serial.read(1024, 2000);  // 2秒超时
    std::cout << "收到: " << response << std::endl;
    
    return 0;
}
```

## 📦 项目集成

### 方法1：系统安装（推荐）
```bash
# 安装到系统
sudo ./build.sh install

# 在你的CMakeLists.txt中使用
find_package(SerialLib REQUIRED)
target_link_libraries(your_app SerialLib::serial_static)
```

### 方法2：子目录方式
```bash
# 复制到你的项目中
cp -r SerialLib /path/to/your/project/third_party/
```

```cmake
# 在你的CMakeLists.txt中
add_subdirectory(third_party/SerialLib)
target_link_libraries(your_app serial_static)
```

### 方法3：手动编译
```bash
g++ -std=c++11 -Ipath/to/SerialLib/include your_app.cpp path/to/SerialLib/build/lib/libserial.a -o your_app
```

### 方法4：FetchContent（自动下载）
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

## 📚 API参考

### 核心操作
```cpp
// 串口管理
bool open(const std::string& device);           // 打开设备
void close();                                   // 关闭设备
bool isOpen() const;                           // 检查是否打开

// 配置
bool configure(BaudRate, DataBits, Parity, StopBits, FlowControl);

// 数据传输
int write(const void* data, size_t size);                   // 快速写入
int write(const void* data, size_t size, bool waitComplete); // 保证传输
bool drain();                                               // 等待传输完成

// 数据接收
int read(void* buffer, size_t size, int timeoutMs = 1000);
std::string read(size_t maxBytes = 1024, int timeoutMs = 1000);

// 缓冲区管理
bool flush(), flushInput(), flushOutput();
int available() const;

// 错误处理
std::string getLastError() const;
```

### 支持的参数
- **波特率**: 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600
- **数据位**: 5, 6, 7, 8
- **校验位**: 无, 奇校验, 偶校验
- **停止位**: 1位, 2位
- **流控制**: 无, 硬件流控(RTS/CTS), 软件流控(XON/XOFF)

## 🔧 数据传输模式

```cpp
// 快速写入（数据进入OS缓冲区）
serial.write("数据");

// 保证传输（等待传输完成）
serial.write("关键数据", true);

// 批量写入后统一保证传输
serial.write("数据块1");
serial.write("数据块2");
serial.drain();  // 确保所有数据已传输
```

## 🛠️ 构建选项

```bash
./build.sh make      # 使用Makefile构建（默认）
./build.sh cmake     # 使用CMake构建
./build.sh clean     # 清理构建文件
./build.sh install   # 安装到系统
./build.sh test      # 运行示例
./build.sh help      # 显示帮助
```

## 📋 系统要求

- Linux操作系统
- GCC 4.8+ 或 Clang 3.4+
- CMake 3.10+（可选）
- C++11标准

## ⚠️ 常见问题

### 1. 权限问题
```bash
sudo usermod -a -G dialout $USER
# 然后重新登录
```

### 2. 设备路径
- USB设备: `/dev/ttyUSB0`, `/dev/ttyUSB1`
- Arduino/CDC: `/dev/ttyACM0`, `/dev/ttyACM1`  
- 传统串口: `/dev/ttyS0`, `/dev/ttyS1`

### 3. 设备忙碌
确保没有其他程序在使用该串口

### 4. CMake集成问题
```cmake
# 如果找不到库，设置路径
set(CMAKE_PREFIX_PATH "/path/to/SerialLib" ${CMAKE_PREFIX_PATH})
find_package(SerialLib REQUIRED)

# 或者指定具体路径
set(SerialLib_DIR "/path/to/SerialLib/lib/cmake/SerialLib")
```

## 📁 项目结构

```
SerialLib/
├── README.md                   # 英文说明
├── README_CN.md                # 中文说明  
├── CMakeLists.txt              # CMake配置
├── Makefile                    # Makefile构建
├── build.sh                    # 构建脚本
├── include/SerialPort.h        # 头文件
├── src/SerialPort.cpp          # 实现文件
├── examples/                   # 使用示例
└── cmake/                      # CMake配置文件
```

## 🔄 卸载

```bash
cd SerialLib/build
sudo make uninstall
# 或手动删除: sudo rm -f /usr/local/lib/libserial.a /usr/local/include/SerialPort.h
```

## 📄 许可证

MIT许可证 - 详见LICENSE文件

## 🤝 贡献

欢迎提交Issue和Pull Request！

---

**English version**: [README.md](README.md) 