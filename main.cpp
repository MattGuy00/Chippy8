#include "Chip8_Emu.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>

int main(int argc, const char** argv) {
	Chip8_Emu emulator {};

	SDL_Window* window;
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Chip8", 640, 320, SDL_WINDOW_OPENGL);

	SDL_DestroyWindow(window);

	emulator.play("rom/IBM_Logo.ch8");

    return 0;
}
