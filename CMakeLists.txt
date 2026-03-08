cmake_minimum_required(VERSION 3.15)
project(ANS)

set(CMAKE_CXX_STANDARD 17)

add_executable(ans_coder
        main.cpp
        priority_queue.cpp
        ANSCoder.cpp)