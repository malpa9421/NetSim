cmake_minimum_required(VERSION 4.1)
project(NetSim)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(NetSim
        main.cpp
        package.cpp
        package.hxx
        storage_types.cpp
        storage_types.hxx
        tests.cpp
        types.hxx)
