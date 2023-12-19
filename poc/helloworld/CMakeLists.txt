cmake_minimum_required(VERSION 3.10.2)
project(hello)

# Specify the C and C++ compilers for QNX
set(CMAKE_C_COMPILER $ENV{QNX_HOST}/usr/bin/aarch64-unknown-nto-qnx7.1.0-gcc)
set(CMAKE_CXX_COMPILER $ENV{QNX_HOST}/usr/bin/aarch64-unknown-nto-qnx7.1.0-g++)

# Create an executable from main.cpp
add_executable(hello main.cpp)
