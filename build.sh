#!/bin/bash

# Serial Communication Library Build Script

set -e  # Exit immediately on error

echo "=== Linux C++ Serial Communication Library Build Script ==="

# Show help information
show_help() {
    echo "Usage: ./build.sh [option]"
    echo ""
    echo "Options:"
    echo "  make      - Build with Makefile (default)"
    echo "  cmake     - Build with CMake"
    echo "  clean     - Clean build files"
    echo "  install   - Install static library to system"
    echo "  test      - Run test program"
    echo "  help      - Show this help information"
    echo ""
    echo "Examples:"
    echo "  ./build.sh make    # Build with Makefile"
    echo "  ./build.sh cmake   # Build with CMake"
    echo "  ./build.sh install # Install static library"
}

# Build with Makefile
build_with_make() {
    echo "Building with Makefile..."
    make clean
    make all
    echo "✓ Build completed!"
    echo "Static library file: build/lib/libserial.a"
    echo "Example program: build/bin/quick_start"
}

# Build with CMake
build_with_cmake() {
    echo "Building with CMake..."
    
    # Check if CMake is available
    if ! command -v cmake &> /dev/null; then
        echo "Error: CMake not found"
        echo "Please install CMake: sudo apt install cmake"
        exit 1
    fi
    
    # Create build directory
    rm -rf build_cmake
    mkdir -p build_cmake
    cd build_cmake
    
    # Configure
    echo "Configuring project..."
    if ! cmake ..; then
        echo "Error: CMake configuration failed"
        echo "Try building with Makefile: ./build.sh make"
        exit 1
    fi
    
    # Build
    echo "Compiling project..."
    make
    
    cd ..
    echo "✓ CMake build completed!"
    echo "Static library file: build_cmake/libserial.a"
    echo "Example program: build_cmake/examples/quick_start"
}

# Clean build files
clean_build() {
    echo "Cleaning build files..."
    rm -rf build build_cmake
    echo "✓ Clean completed!"
}

# Install library
install_lib() {
    if [ ! -f "build/lib/libserial.a" ]; then
        echo "Error: Built static library file not found"
        echo "Please run build first: ./build.sh make"
        exit 1
    fi
    
    echo "Installing static library to system..."
    make install
    echo "✓ Installation completed!"
}

# Run tests
run_tests() {
    if [ ! -f "build/bin/quick_start" ]; then
        echo "Error: Test program not found"
        echo "Please run build first: ./build.sh make"
        exit 1
    fi
    
    echo "Running test program..."
    make test
}

# Main logic
case "${1:-make}" in
    "make")
        build_with_make
        ;;
    "cmake")
        build_with_cmake
        ;;
    "clean")
        clean_build
        ;;
    "install")
        install_lib
        ;;
    "test")
        run_tests
        ;;
    "help"|"--help"|"-h")
        show_help
        ;;
    *)
        echo "Error: Unknown option '$1'"
        echo ""
        show_help
        exit 1
        ;;
esac 