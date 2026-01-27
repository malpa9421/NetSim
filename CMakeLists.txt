cmake_minimum_required(VERSION 4.1)
project(NetSim)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
include_directories(include)

add_executable(NetSim
        main.cpp
        src/package.cpp
        include/package.hxx
        src/storage_types.cpp
        include/storage_types.hxx
        tests/tests.cpp
        include/types.hxx
        src/helpers.cpp
        src/nodes.cpp
        include/nodes.hxx
        src/factory.cpp)