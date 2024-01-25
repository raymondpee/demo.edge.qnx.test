# Set the minimum required CMake version
cmake_minimum_required(VERSION 3.12)

# Define your project and specify its name
project(MyProject)

# Define the C++ standard (e.g., C++11, C++14, C++17)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64 -fPIC -O2 -DSOCKLEN_T=socklen_t -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -m64 -fPIC -Wall -DBSD=1 -DSOCKLEN_T=socklen_t -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64")

set(DIR_BASIC_USAGE ${CMAKE_CURRENT_SOURCE_DIR}/BasicUsageEnvironment/)
set(DIR_USAGE ${CMAKE_CURRENT_SOURCE_DIR}/UsageEnvironment/)
set(DIR_HLS_PROXY ${CMAKE_CURRENT_SOURCE_DIR}/hlsProxy/)
set(DIR_LIVE_MEDIA ${CMAKE_CURRENT_SOURCE_DIR}/liveMedia/)
set(DIR_GROUP_SOCK ${CMAKE_CURRENT_SOURCE_DIR}/groupsock/)
set(DIR_MEDIA_SERVER ${CMAKE_CURRENT_SOURCE_DIR}/mediaServer/)
set(DIR_PROXY_SERVER ${CMAKE_CURRENT_SOURCE_DIR}/proxyServer/)

find_package(OpenCV REQUIRED)

# Include multiple directories
include_directories(
    ${DIR_GROUP_SOCK}/include
    ${DIR_USAGE}/include
    ${DIR_BASIC_USAGE}/include
    ${DIR_LIVE_MEDIA}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/testProgs
)

file(GLOB FILE_BASIC_USAGE "${DIR_BASIC_USAGE}/*.c*")
file(GLOB FILE_GROUP_SOCK "${DIR_GROUP_SOCK}/*.c*")
file(GLOB FILE_LIVE_MEDIA "${DIR_LIVE_MEDIA}/*.c*")
file(GLOB FILE_USAGE "${DIR_USAGE}/*.cpp")

# Add an executable target and list all source files
add_executable(RTSPClient
    ${FILE_BASIC_USAGE}     
    ${FILE_GROUP_SOCK}              
    ${FILE_LIVE_MEDIA}         
    ${FILE_USAGE}         
    "${CMAKE_CURRENT_SOURCE_DIR}/src/RTSPClient.cpp"         
)

target_link_libraries(RTSPClient ssl crypto ${OpenCV_LIBS}) 