cmake_minimum_required(VERSION 3.0)
project(udp_processor)

# Set the compiler paths for Ubuntu
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC ")

find_package(OpenCV REQUIRED)

set(OPENCV_INCLUDE_DIR /usr/include/opencv4)
set(SOCKET_INCLUDE_DIR /usr/include)
set(SNPE_INCLUDE_DIR $ENV{SNPE_ROOT}/include/SNPE)
set(SNPE_LIBRARY_DIR $ENV{SNPE_ROOT}/lib/x86_64-linux-clang)
set(GNU_LIBRARY_DIR /usr/lib/x86_64-linux-gnu)

include_directories(${SOCKET_INCLUDE_DIR} ${SNPE_INCLUDE_DIR} ${OPENCV_INCLUDE_DIR}) 

add_executable(udp_processor main.cpp)

target_link_libraries(udp_processor 
    ${SNPE_LIBRARY_DIR}/libSNPE.so
    ${GNU_LIBRARY_DIR}/libopencv_core.so.4.2
    ${GNU_LIBRARY_DIR}/libopencv_imgcodecs.so.4.2
    ${GNU_LIBRARY_DIR}/libopencv_imgproc.so.4.2
    ${GNU_LIBRARY_DIR}/libc++.so.1
)
