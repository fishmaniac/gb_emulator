# gcc main.c cpu.c tests.c -Wall -g -Og -lm -DTESTS -o gb_emulator
cd build
cmake .. -DCMAKE_C_COMPILER=gcc -DCMAKE_EXPORT_COMPILE_COMMANDS=1
make
cp build/compile_commands.json .
