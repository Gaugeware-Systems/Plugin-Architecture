cmake_minimum_required(VERSION 3.20)
project(cpp20_plugin_architecture CXX)

include(CTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()

# Compiler warning flags
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Output all runtime artifacts into standard build bin/plugins directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins)

# Find required libraries
find_package(Boost REQUIRED COMPONENTS program_options)
find_package(spdlog REQUIRED)

# Include directories
include_directories(include)

# Host application
add_executable(host_app src/main.cpp)
target_link_libraries(host_app PRIVATE ${CMAKE_DL_LIBS})
target_link_libraries(host_app PRIVATE Boost::program_options)
target_link_libraries(host_app PRIVATE spdlog::spdlog)

# Plugins
add_subdirectory(plugins)

if (BUILD_TESTING)
    add_test(NAME plugin_load
        COMMAND host_app "$<TARGET_FILE:sample_plugin>"
    )
endif()
