cmake_minimum_required(VERSION 4.0)
project(MinKV)

set(CMAKE_CXX_STANDARD 23)

add_executable(MinKV main.cpp
        request_response_server.cpp
        parser.cpp
        parser.h)
