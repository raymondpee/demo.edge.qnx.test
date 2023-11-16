cmake_minimum_required(VERSION 3.12)
project(MyRestApiServer)

set(CMAKE_CXX_STANDARD 14)

find_package(cpprestsdk REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE cpprestsdk::cpprest)