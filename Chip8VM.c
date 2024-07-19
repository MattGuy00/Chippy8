#include "Chip8VM.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <byteswap.h>

#define DISASSEMBLE
// Loads rom from disk into the vm's memory
void loadROM(struct chip8_vm* vm, const char* romPath) {
    int fd = open(romPath, O_RDWR, S_IRWXU | S_IRGRP);
    if (fd < 0) {
        perror("Unable to open ROM");
        exit(EXIT_FAILURE);
    }

    struct stat fileInfo = {0};
    if (fstat(fd, &fileInfo) < 0) {
        perror("Error retrieving file info");
        exit(EXIT_FAILURE);
    }

    if (mmap(&(vm->memory[0x200]), fileInfo.st_size,
             PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE, fd, 0) == MAP_FAILED) {
        perror("Error mapping rom to memory");
        exit(EXIT_FAILURE);
    }

#ifdef DISASSEMBLE
    uint16_t* rom = malloc(fileInfo.st_size);
    read(fd, rom, fileInfo.st_size);
    for (int i = 0; i < fileInfo.st_size; ++i) {
        if (i % 4 == 0) {
            printf("\n");
        }

        // Converts from be to le
        printf("0x%x ",__bswap_constant_16(rom[i]));
    }
    free(rom);
#endif

    close(fd);
}