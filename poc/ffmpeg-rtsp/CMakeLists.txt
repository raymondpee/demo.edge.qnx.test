cmake_minimum_required(VERSION 3.10)
project(H264_Decoder)

# Add the executable target
add_executable(h264_decoder main.cpp)

if(${PLATFORM} STREQUAL "x64")
    set(FFMPEG_LIBRARY_DIRS /usr/lib/x86_64-linux-gnu)
    set(FFMPEG_INCLUDE_DIRS /usr/include/x86_64-linux-gnu)
    target_link_libraries(h264_decoder 
        ${FFMPEG_LIBRARY_DIRS}/libavcodec.so.58.54.100 
        ${FFMPEG_LIBRARY_DIRS}/libavformat.so.58.29.100 
        ${FFMPEG_LIBRARY_DIRS}/libavutil.so.56.31.100 
        ${FFMPEG_LIBRARY_DIRS}/libswscale.so.5.5.100
    )
    target_include_directories(h264_decoder PRIVATE ${FFMPEG_INCLUDE_DIRS})
elseif(${PLATFORM} STREQUAL "qnx")
    set(CMAKE_C_COMPILER $ENV{QNX_HOST}/usr/bin/aarch64-unknown-nto-qnx7.1.0-gcc)
    set(CMAKE_CXX_COMPILER $ENV{QNX_HOST}/usr/bin/aarch64-unknown-nto-qnx7.1.0-g++)
    set(FFMPEG_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/FFmpeg)
    target_link_libraries(h264_decoder 
    ${CMAKE_CURRENT_SOURCE_DIR}/FFmpeg/libavcodec/libavcodec.so 
    ${CMAKE_CURRENT_SOURCE_DIR}/FFmpeg/libavformat/libavformat.so 
    ${CMAKE_CURRENT_SOURCE_DIR}/FFmpeg/libavutil/libavutil.so 
    ${CMAKE_CURRENT_SOURCE_DIR}/FFmpeg/libswscale/libswscale.so
    ${CMAKE_CURRENT_SOURCE_DIR}/FFmpeg/libswresample/libswresample.so
    )
    target_include_directories(h264_decoder PRIVATE ${FFMPEG_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Unsupported platform specified with -DPLATFORM")
endif()




