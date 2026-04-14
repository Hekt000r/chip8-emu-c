#include "instructions.h"
#include <stdint.h>
#include <stdio.h>

// system family includes 2 instructions: disp_clear and return.
// they are differentiated by their low nibble (last 4bits)
// disp_clear LN: 0, return LN: E
// so if low nibble is 0 then its a disp_clear otherwise its a subr_return
int op_system_family(uint16_t instruction) { // disp_clear and subr_return
    uint8_t lowNibble = instruction & 0x000F;

    // now we can split the logic
    if (lowNibble == 0) {
        printf("disp_clear\n");
    } else {
        printf("subr_return \n");
        // TODO: return 0 and instead directly set the PC to the address stored in the stack
    }

    return 2;
} 