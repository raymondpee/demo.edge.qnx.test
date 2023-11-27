cmake_minimum_required(VERSION 3.10.2)
project(Rest_client)

set(CMAKE_C_COMPILER $ENV{QNX_HOST}/usr/bin/aarch64-unknown-nto-qnx7.1.0-gcc)
set(CMAKE_CXX_COMPILER $ENV{QNX_HOST}/usr/bin/aarch64-unknown-nto-qnx7.1.0-g++)

set(TARGET_DIR $ENV{QNX_HOST}/../../../target/qnx7)
set(CURL_INCLUDE_DIR ${TARGET_DIR}/usr/include)
set(CURL_LIBRARY ${TARGET_DIR}/aarch64le/usr/lib/libcurl.so)

include_directories(${CURL_INCLUDE_DIR})

add_executable(rest_client main.cpp)

target_link_libraries(rest_client ${CURL_LIBRARY})

set(SOURCE_DIR ${TARGET_DIR}/aarch64le/usr/lib/)

set(DEST_DIR ${CMAKE_BINARY_DIR})

set(DEPENDENCY_LIBS
    libcurl.so.12
    libcares.so.8
)

foreach(file ${DEPENDENCY_LIBS})
    file(COPY ${SOURCE_DIR}/${file} DESTINATION ${DEST_DIR})
endforeach()