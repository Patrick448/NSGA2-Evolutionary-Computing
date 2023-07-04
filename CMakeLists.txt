cmake_minimum_required(VERSION 3.20)
project(Green_Scheduling_Distributed_Flowshop_Problem)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "-O3")
SET(CMAKE_C_FLAGS "-O3")

include_directories(include)

add_executable(Green_Scheduling_Distributed_Flowshop_Problem
        include/defines.hpp
        include/Factory.hpp
        include/Job.hpp
        include/Individual.hpp
        include/Problem.hpp
        include/read.hpp
        source/Factory.cpp
        source/Job.cpp
        source/main.cpp
        source/Problem.cpp
        source/read.cpp
        source/Individual.cpp
        include/Util.hpp
        source/Util.cpp
        source/NSGAII.cpp
        include/NSGAII.hpp)
