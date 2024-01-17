cmake_minimum_required(VERSION 3.10.2)
project(udp_processor)

# Set the compiler paths for Ubuntu
set(CMAKE_C_COMPILER /opt/gcc-linaro-4.9-2014.11-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER /opt/gcc-linaro-4.9-2014.11-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-g++)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -ldl -pthread")

# Specify the paths for the socket library on Ubuntu
set(SOCKET_INCLUDE_DIR /usr/include)
set(FASTCV_INCLUDE_DIR /opt/fastcv/fastcv-1-7-1_LinuxEmbedded/inc)
set(FASTCV_LIBRARY /opt/fastcv/fastcv-1-7-1_LinuxEmbedded/lib/64-bit/libfastcv.a)

include_directories(${SOCKET_INCLUDE_DIR} ${FASTCV_INCLUDE_DIR}) 

add_executable(udp_processor main.cpp)
target_link_libraries(udp_processor ${FASTCV_LIBRARY})

