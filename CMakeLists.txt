cmake_minimum_required(VERSION 4.0)
project(MinKV)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenSSL REQUIRED CONFIG)

add_executable(MinKV src/main.cpp
        src/rr_server.cpp
        src/parser.cpp
        src/parser.h
        src/min_kv_server.cpp
        src/min_kv_server.h
        src/session.cpp
        src/session.h
        src/ssl_session.cpp
        src/ssl_session.h)

target_link_libraries(MinKV ssl crypto)
