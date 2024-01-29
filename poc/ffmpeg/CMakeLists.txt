cmake_minimum_required(VERSION 3.10)
project(H264_Decoder)

# Add the executable target
add_executable(h264_decoder main.cpp)

set(GNU_LIBRARY_DIR /usr/lib/x86_64-linux-gnu)
set(FFMPEG_LIBRARY_DIRS /usr/lib/x86_64-linux-gnu)
set(FFMPEG_INCLUDE_DIRS /usr/include/x86_64-linux-gnu)
set(OPENCV_INCLUDE_DIR /usr/include/opencv4)


# Link FFmpeg libraries
target_link_libraries(h264_decoder 
    ${FFMPEG_LIBRARY_DIRS}/libavcodec.so.58.54.100 
    ${FFMPEG_LIBRARY_DIRS}/libavformat.so.58.29.100 
    ${FFMPEG_LIBRARY_DIRS}/libavutil.so.56.31.100 
    ${FFMPEG_LIBRARY_DIRS}/libswscale.so.5.5.100
    ${GNU_LIBRARY_DIR}/libopencv_core.so.4.2
    ${GNU_LIBRARY_DIR}/libopencv_imgcodecs.so.4.2
    ${GNU_LIBRARY_DIR}/libopencv_imgproc.so.4.2
)

# Set include directories for FFmpeg headers
target_include_directories(h264_decoder PRIVATE ${FFMPEG_INCLUDE_DIRS} ${OPENCV_INCLUDE_DIR})



