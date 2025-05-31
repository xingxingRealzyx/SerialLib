# CMake configuration for example programs

# Quick start example
add_executable(quick_start quick_start.cpp)
target_link_libraries(quick_start serial_static)

# Set output directory for example programs
set_target_properties(quick_start PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
) 