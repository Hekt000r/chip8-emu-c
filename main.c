#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <signal.h>
#include "instructions.h"
#include "display.h"

/* --= Hardware =-- */
uint8_t RAM[4096] = {
    0,0
}; // 4 KB CHIP-8 RAM

// Internal Register: Stack Pointer for the stack
uint8_t stack_pointer = 0; //0-11

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

uint16_t stack[12]; // original RCA1802 had 12 levels of stack

// CPU

const int target_hz = 10;
const int ms_per_instruction = 1000 / target_hz; //2ms


bool isRunning = true;

/* ---==== INSTRUCTIONS ====---*/

typedef int (*Handler)(uint16_t);

// system family includes 2 instructions: disp_clear and return.
// they are differentiated by their low nibble (last 4bits)
// disp_clear LN: 0, return LN: E, halt LN: D
// so if low nibble is 0 then its a disp_clear otherwise its a subr_return
int op_system_family(uint16_t instruction) { // disp_clear and subr_return
    uint8_t lowNibble = instruction & 0x000F;

    // ignore empty space / padding
    if (instruction == 0x0000) {
        return 0;
    }

    // now we can split the logic
    if (lowNibble == 0) { // disp_clear
        printf("[EXEC] disp_clear\n");
        display_clear();
    } else if (lowNibble == 0xD) { // halt
        printf("[EXEC] halt\n");
        isRunning = false;
        return 0;
    } else { // return from sub routine | RET
        // safety check: error for stack underflow
        if (stack_pointer == 0) {
            printf("[ERROR_EXEC]: Stack underflow error\n");
            isRunning = false;
            return 0;
        }
        stack_pointer = stack_pointer - 1;
        program_counter = stack[stack_pointer];
        return 0;
    }

    return 2;
} 

int op_goto(uint16_t instruction) {
    // We need the last 3 hex digits / last 12 bits to get NNN (jump address)
    uint16_t jump_address = instruction & 0x0FFF;
    program_counter = jump_address;
    return 0;
}

int op_call_subroutine(uint16_t instruction) {
    // Get 3 nibble (12 bits/3 hex)
    uint16_t subroutine_address = instruction & 0x0FFF;
    stack[stack_pointer] = program_counter + 2;
    stack_pointer = stack_pointer + 1;
    program_counter = subroutine_address;
    return 0;
}

Handler opcodes[256] = {
    [0] = op_system_family,
    [1] = op_goto,
    [2] = op_call_subroutine
};


// Fetch-Decode-Execute Cycle

void handle_interrupt(int sig) {
    isRunning = false;
}

int main()
{
    // --== Initialize Hardware ==--
    // Display
    display_init();
    
    // Test ROM loader
    RAM[0x200] = 0x22; RAM[0x201] = 0x06; // CALL 0x206
    RAM[0x202] = 0x12; RAM[0x203] = 0x02; // JP 0x202
    RAM[0x206] = 0x00; RAM[0x207] = 0xE0; // CLS
    RAM[0x208] = 0x00; RAM[0x209] = 0xEE; // RET

    while (isRunning == true)
    {
        // detect control+c signal
        signal(SIGINT, handle_interrupt);
        uint32_t start_time = display_get_ticks();
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

        if (start_time % 16 == 0) {
            display_render();
        }

        uint32_t elapsed = display_get_ticks() - start_time;
        if (elapsed < ms_per_instruction) {
            display_delay(ms_per_instruction - elapsed);
        }
    }

    display_shutdown();
    return 0;
}
