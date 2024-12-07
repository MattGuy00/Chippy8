#include "Chip8_Emu.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>

int main(int argc, const char** argv) {
	// TODO: Add more error checking for argv
	if (argc != 2) {
		std::cout << "Usage: ./Chippy8 [path_to_rom]\n";
		return 1;
	}
	
	Chip8_Emu emulator {};

	emulator.play(argv[1]);

    return 0;
}
