cmake_minimum_required(VERSION 3.27)
project(lil_jvm C)

set(CMAKE_C_STANDARD 11)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

add_executable(lil_jvm main.c
        debug.c
        class.c
        memory.c
        vm.c
        vm.h
        native.c
        native.h)
