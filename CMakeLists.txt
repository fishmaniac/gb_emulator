cmake_minimum_required(VERSION 3.20)

project(gb_emulator C)

set(C_STANDARD 23)
set(C_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -g -Og -lm -DTESTS)
add_compile_options(-Wpedantic)
# add_compile_options(-Werror)

add_executable(gb_emulator
	main.c
	cpu.c
	memory.c
	tests.c
)

target_link_libraries(gb_emulator m)
