cmake_minimum_required(VERSION 3.10.2)
project(Rest_client)


#set(CMAKE_TOOLCHAIN_FILE ./../../toolchain/qnx710.cmake)

set(CMAKE_C_COMPILER $ENV{QNX_HOST}/usr/bin/aarch64-unknown-nto-qnx7.1.0-gcc)
set(CMAKE_CXX_COMPILER $ENV{QNX_HOST}/usr/bin/aarch64-unknown-nto-qnx7.1.0-g++)

set(CURL_INCLUDE_DIR /opt/qnx710/qnx710/target/qnx7/usr/include)
set(CURL_LIBRARY /opt/qnx710/qnx710/target/qnx7/aarch64le/usr/lib/libcurl.so)

include_directories(${CURL_INCLUDE_DIR})

add_executable(rest_client main.cpp)

target_link_libraries(rest_client ${CURL_LIBRARY})
