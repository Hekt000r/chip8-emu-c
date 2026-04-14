#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "instructions.h"

/* Hardware */
uint8_t RAM[4096] = {
    0,0
}; // 4 KB CHIP-8 RAM

// General Purpose Registers
uint8_t V0 = 0;
uint8_t V1 = 0;
uint8_t V2 = 0;
uint8_t V3 = 0;
uint8_t V4 = 0;
uint8_t V5 = 0;
uint8_t V6 = 0;
uint8_t V7 = 0;
uint8_t V8 = 0;
uint8_t V9 = 0;
uint8_t V10 = 0;
uint8_t VA = 0;
uint8_t VB = 0;
uint8_t VC = 0;
uint8_t VD = 0;
uint8_t VE = 0;
uint8_t VF = 0;

uint16_t index_register = 0;
uint16_t program_counter = 0x200; // first 512 bytes are reserved for interpreter usually

uint8_t delay = 60;
uint8_t sound = 60;

uint16_t stack[48]; // original RCA1802 had 48 bytes of stack

bool isRunning = true;

// Instructions

typedef int (*Handler)(uint16_t);

Handler opcodes[256] = {
    [0] = op_system_family 
};

// Fetch-Decode-Execute Cycle

int main()
{
    // Test ROM loader
    RAM[0x200] = 0x00;
    RAM[0x201] = 0xE0;
    // full instruction: 0x00E0 (CLR / disp_clear)

    while (isRunning == true)
    {
        // Fetch
        uint16_t instruction = ((uint16_t)RAM[program_counter] << 8) | RAM[program_counter + 1];
        //                       ^^ We have to use type casting or shitfing 8 bits
        // to the left will cause the bits to be "lost" instead of "making room"

        // 1. Grab the opcode 2. Shift the 8 bits to the left to make room for the operand 3. Fetch the operand and place it in
        // the place of empty bits.
        // We need the first 4 bits / the first hexadecimal character to index the
        // opcodes array, since instructions are grouped based on their first 4 bits
        uint8_t highNibble = instruction >> 12; // Shifts the other 12 bits to the right so it leaves only the first 4 bits
        printf("[high nibble]: %d\n",highNibble);

        // Decode
        if (opcodes[highNibble] != NULL) {
            int step = opcodes[highNibble](instruction);
            program_counter = program_counter + step;
        } else {
            printf("Unknown high nibble/instruction: %d\n", instruction);
            isRunning = false;
        }

        printf("[instruction]: %d\n", instruction);

        printf("[program_counter]: %d\n", program_counter);
    }
}
