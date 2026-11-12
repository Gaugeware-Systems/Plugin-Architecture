# This is the toolchain file for building with clang/llvm on Linux. It sets the
# system name to LLVM and specifies the C++ compiler to be g++.
set(CMAKE_SYSTEM_NAME Linux-LLVM)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
