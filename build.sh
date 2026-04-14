#!/bin/bash
echo "[COMPILE] Compiling source ..."
gcc main.c display.c -lSDL2 -o build/emu.o

echo "[FINISH] Built successfully."
