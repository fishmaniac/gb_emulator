#!/bin/bash
./build.sh
valgrind --leak-check=full -s ./build/gb_emulator
rm build
