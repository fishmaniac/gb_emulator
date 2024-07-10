#!/bin/bash
./build.sh
valgrind --leak-check=full ./build/gb_emulator
rm build
