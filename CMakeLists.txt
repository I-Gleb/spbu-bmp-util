cmake_minimum_required(VERSION 3.16.3)

project(hw-01_bmp C)

add_executable(hw-01_bmp src/main.c src/bmp.c src/stego.c include/bmp.h include/stego.h)

include_directories(include/)
set(CMAKE_CXX_FLAGS -Wall -Wextra -Werror)

