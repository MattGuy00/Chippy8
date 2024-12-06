#include "Chip8_Emu.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>

int main(int argc, const char** argv) {
	Chip8_Emu emulator {};


	emulator.play("rom/IBM_Logo.ch8");

    return 0;
}
