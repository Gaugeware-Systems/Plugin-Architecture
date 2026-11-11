cmake_minimum_required(VERSION 3.20)
project(cpp20_plugin_architecture CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warning flags
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Output all runtime artifacts into standard build bin/plugins directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins)

# Include directories
include_directories(include)

# Host application
add_executable(host_app src/main.cpp)
target_link_libraries(host_app PRIVATE ${CMAKE_DL_LIBS})

# Plugins
add_subdirectory(plugins)
