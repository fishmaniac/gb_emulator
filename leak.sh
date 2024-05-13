gcc main.c cpu.c -Wall -g -Og -lm -DTESTS -o gbc_emu
./gbc_emu
valgrind --leak-check=full ./gbc_emu
rm gbc_emu
