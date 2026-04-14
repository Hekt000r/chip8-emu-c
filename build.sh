echo "[COMPILE] Compiling source ..."
gcc main.c instructions.c -o build/emu.o

echo "[RUN] Running executable ..."
./build/emu.o
