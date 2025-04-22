在MacOS上创建，使用C++17编译。

下面是CMakeLists.txt的内容。

cmake_minimum_required(VERSION 3.29)
project(pdp_bmprotate)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(pdp_bmprotate main.cpp)
