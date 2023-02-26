cmake_minimum_required(VERSION 3.11)
set (CMAKE_CXX_STANDARD 20)

project(TCPServer
  LANGUAGES CXX
  VERSION 1.0.0
)

add_executable(TCPServer src/main.cpp src/TCPServer.cpp)
target_include_directories(TCPServer PRIVATE src)