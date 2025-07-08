cmake_minimum_required(VERSION 3.10)
project(clox)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

include_directories(include)

add_executable(main
        src/main.cpp
)