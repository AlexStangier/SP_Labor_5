cmake_minimum_required(VERSION 3.19)
project(lab5 C)
set(CMAKE_C_STANDARD 11)
include_directories(.)
add_executable(Run Test.c HeapManager.c HeapManagerPrivate.h)



