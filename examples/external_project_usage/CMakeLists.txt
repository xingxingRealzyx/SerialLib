cmake_minimum_required(VERSION 3.10)
project(ExternalProjectExample VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Method 1: Use find_package (if SerialLib is installed system-wide)
# Uncomment the following lines if SerialLib is installed:
# find_package(SerialLib REQUIRED)
# add_executable(my_app main.cpp)
# target_link_libraries(my_app SerialLib::serial_static)

# Method 2: Add as subdirectory (if SerialLib source is available)
# Assuming SerialLib source is in the parent directory
set(SERIALLIB_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")

# Check if SerialLib source exists
if(EXISTS "${SERIALLIB_SOURCE_DIR}/CMakeLists.txt")
    message(STATUS "Found SerialLib source at: ${SERIALLIB_SOURCE_DIR}")
    
    # Add SerialLib as subdirectory
    add_subdirectory(${SERIALLIB_SOURCE_DIR} seriallib_build)
    
    # Create example application
    add_executable(my_app main.cpp)
    
    # Link with SerialLib
    target_link_libraries(my_app serial_static)
    
    message(STATUS "Building with SerialLib as subdirectory")
else()
    message(WARNING "SerialLib source not found. Please either:")
    message(WARNING "1. Install SerialLib system-wide and use find_package")
    message(WARNING "2. Place SerialLib source in the correct location")
    message(WARNING "3. Update SERIALLIB_SOURCE_DIR variable")
endif()

# Method 3: Use FetchContent (download from repository)
# Uncomment and modify the following if you want to download SerialLib automatically:
#
# include(FetchContent)
# FetchContent_Declare(
#     SerialLib
#     GIT_REPOSITORY https://github.com/xingxingRelzyx/SerialLib.git
#     GIT_TAG        main  # or specific version tag
# )
# FetchContent_MakeAvailable(SerialLib)
# add_executable(my_app main.cpp)
# target_link_libraries(my_app serial_static)

# Print usage information
message(STATUS "")
message(STATUS "=== SerialLib Integration Example ===")
message(STATUS "This example demonstrates different ways to use SerialLib in your project:")
message(STATUS "")
message(STATUS "Method 1: System-wide installation")
message(STATUS "  - Install SerialLib: cd /path/to/SerialLib && mkdir build && cd build")
message(STATUS "  - cmake .. && make && sudo make install")
message(STATUS "  - Use: find_package(SerialLib REQUIRED)")
message(STATUS "")
message(STATUS "Method 2: Add as subdirectory")
message(STATUS "  - Copy SerialLib source to your project")
message(STATUS "  - Use: add_subdirectory(path/to/SerialLib)")
message(STATUS "")
message(STATUS "Method 3: FetchContent (automatic download)")
message(STATUS "  - Use: FetchContent_Declare + FetchContent_MakeAvailable")
message(STATUS "") 