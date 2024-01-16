cmake_minimum_required(VERSION 3.0)
project(SNPE_TEST)

# Set the compiler paths for Ubuntu
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC ")

find_package(OpenCV REQUIRED)

set(OPENCV_INCLUDE_DIR /usr/include/opencv4)
set(SNPE_INCLUDE_DIR $ENV{SNPE_ROOT}/include/SNPE)
set(SNPE_LIBRARY_DIR $ENV{SNPE_ROOT}/lib/x86_64-linux-clang)
set(GNU_LIBRARY_DIR /usr/lib/x86_64-linux-gnu)

include_directories(${SNPE_INCLUDE_DIR} ${OPENCV_INCLUDE_DIR}) 

add_executable(snpe_exec main.cpp ppyoloe_snpe_engine.cpp)

target_link_libraries(snpe_exec 
    ${SNPE_LIBRARY_DIR}/libSNPE.so
    ${GNU_LIBRARY_DIR}/libopencv_core.so.4.2
    ${GNU_LIBRARY_DIR}/libopencv_imgcodecs.so.4.2
    ${GNU_LIBRARY_DIR}/libopencv_imgproc.so.4.2
    ${GNU_LIBRARY_DIR}/libc++.so.1
)
