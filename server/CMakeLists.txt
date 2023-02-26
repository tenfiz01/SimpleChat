cmake_minimum_required(VERSION 3.24)
set (CMAKE_CXX_STANDARD 20)

project(TCPServer)

add_executable(TCPServer src/main.cpp src/TCPServer.cpp)