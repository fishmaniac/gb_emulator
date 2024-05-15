# gcc main.c cpu.c tests.c -Wall -g -Og -lm -DTESTS -o gbc_emu
cd build
cmake .. -DCMAKE_C_COMPILER=gcc
make
