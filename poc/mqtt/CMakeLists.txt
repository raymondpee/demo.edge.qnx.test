cmake_minimum_required(VERSION 3.10.2)
project(mqtt)

# Specify the C and C++ compilers for QNX
set(CMAKE_C_COMPILER $ENV{QNX_HOST}/usr/bin/aarch64-unknown-nto-qnx7.1.0-gcc)
set(CMAKE_CXX_COMPILER $ENV{QNX_HOST}/usr/bin/aarch64-unknown-nto-qnx7.1.0-g++)

# Create an executable from main.cpp
add_executable(mqtt_pub main_pub.cpp)
add_executable(mqtt_sub main_sub.cpp)

set(MOSQUITTO_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/mosquitto/include)
set(MOSQUITTO_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/mosquitto/lib/libmosquitto.so.1)

set(DEPENDENCY_LIBS
    libmosquitto.so.1
)

# Move target_link_libraries commands here

include_directories(${MOSQUITTO_INCLUDE_DIR})

target_link_libraries(mqtt_pub ${MOSQUITTO_LIBRARY})
target_link_libraries(mqtt_sub ${MOSQUITTO_LIBRARY})

set(SOURCE_DIR mosquitto/lib)
set(DEST_DIR ${CMAKE_BINARY_DIR})

foreach(file ${DEPENDENCY_LIBS})
    file(COPY ${SOURCE_DIR}/${file} DESTINATION ${DEST_DIR})
endforeach()
