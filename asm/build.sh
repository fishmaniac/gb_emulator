rm test_program.o
rm test_program.gb
rgbasm test_program.asm -o test_program.o
rgblink test_program.o -o test_program.gb
