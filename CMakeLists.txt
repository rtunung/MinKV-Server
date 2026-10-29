cmake_minimum_required(VERSION 4.0)
project(MinKV)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenSSL REQUIRED CONFIG)
find_package(TBB REQUIRED CONFIG)
find_package(SQLite3)

add_executable(MinKV src/main.cpp
        src/rr_server.cpp
        src/parser.cpp
        src/parser.h
        src/min_kv_server.cpp
        src/min_kv_server.h
        src/session.cpp
        src/session.h
        src/ssl_session.cpp
        src/ssl_session.h
        src/ssl_rr_server.cpp
        src/ssl_rr_server.h
        src/key_value_storage.cpp
        src/key_value_storage.h
        src/config.cpp
        src/config.h
        src/user_db_connection.cpp
        src/user_db_connection.h
        src/crypto.cpp
        src/crypto.h)

target_link_libraries(MinKV ssl crypto TBB::tbb SQLite3::SQLite3)

add_custom_command(
        TARGET MinKV
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy_if_different
        ${CMAKE_SOURCE_DIR}/default.cfg
        $<TARGET_FILE_DIR:MinKV>/config.cfg
)
