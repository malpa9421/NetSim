cmake_minimum_required(VERSION 3.30.5)
project(NetSim)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(NetSim
        main.cpp
        src/package.cpp
        include/package.hxx
        src/storage_types.cpp
        include/storage_types.hxx
        tests/tests.cpp
        tests/types.hxx)
