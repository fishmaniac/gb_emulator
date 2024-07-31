#!/bin/bash
./build.sh
gdb ./build/gb_emulator
rm -rf build
