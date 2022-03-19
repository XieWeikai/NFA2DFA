cmake_minimum_required(VERSION 3.17)
project(NFA2DFA C)

set(CMAKE_C_STANDARD 11)

add_executable(NFA2DFA
        NFA.c bitarray.c DFA.c convert.c main.c)
