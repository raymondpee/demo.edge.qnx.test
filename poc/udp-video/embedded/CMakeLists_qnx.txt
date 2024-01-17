cmake_minimum_required(VERSION 3.10.2)
project(udp_processor)

set(CMAKE_C_COMPILER $ENV{QNX_HOST}/usr/bin/aarch64-unknown-nto-qnx7.1.0-gcc)
set(CMAKE_CXX_COMPILER $ENV{QNX_HOST}/usr/bin/aarch64-unknown-nto-qnx7.1.0-g++)

set(TARGET_DIR $ENV{QNX_HOST}/../../../target/qnx7)
set(SOCKET_INCLUDE_DIR ${TARGET_DIR}/usr/include)
set(SOCKET_LIBRARY ${TARGET_DIR}/aarch64le/lib/libsocket.so.3)

include_directories(${SOCKET_INCLUDE_DIR})

add_executable(udp_processor main.cpp)

target_link_libraries(udp_processor ${SOCKET_LIBRARY})

set(SOURCE_DIR ${TARGET_DIR}/aarch64le/lib/)

set(DEST_DIR ${CMAKE_BINARY_DIR})

set(DEPENDENCY_LIBS
    libsocket.so.3
)

foreach(file ${DEPENDENCY_LIBS})
    file(COPY ${SOURCE_DIR}/${file} DESTINATION ${DEST_DIR})
endforeach()