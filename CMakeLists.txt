cmake_minimum_required(VERSION 3.27)
project(lil_jvm C)

set(CMAKE_C_STANDARD 11)

add_executable(lil_jvm main.c
        debug.c
        loader.c
        memory.c
        vm.c
        vm.h)
