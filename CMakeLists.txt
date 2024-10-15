cmake_minimum_required(VERSION 3.20)

project(gb_emulator C)

set(C_STANDARD 23)
set(C_STANDARD_REQUIRED ON)

add_compile_options(-lm -DTESTS)
add_compile_options(-Wall -Wextra)
add_compile_options(-g -O0)
add_compile_options(-Wpedantic)
# add_compile_options(-Werror)

find_package(SDL2 REQUIRED)

add_executable(gb_emulator
    main.c
    cpu.c
    memory.c
    tests.c
    window.c
    timer.c
)

target_link_libraries(gb_emulator m SDL2)

target_include_directories(gb_emulator PRIVATE ${SDL2_INCLUDE_DIRS})
