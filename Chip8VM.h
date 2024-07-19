#ifndef CHIPPY8_CHIP8VM_H
#define CHIPPY8_CHIP8VM_H

#include <stdint.h>

#define CHIP8_MEMORY_SIZE 4096
#define CHIP8_SCREEN_WIDTH 64
#define CHiP8_SCREEN_HEIGHT 32
#define CHIP8_STACK_SIZE 16
#define CHIP8_REG_MAX 16

struct chip8_vm {
    uint8_t memory[CHIP8_MEMORY_SIZE];
    uint8_t display[CHIP8_SCREEN_WIDTH * CHiP8_SCREEN_HEIGHT];
    uint16_t PC;
    uint16_t I;
    uint16_t stack[CHIP8_STACK_SIZE];
    uint8_t delayTimer;
    uint8_t soundTimer;
    uint8_t V[CHIP8_REG_MAX];
};

void loadROM(struct chip8_vm* vm, const char* romPath);

#endif
