#include "Chip8_Emu.h"
#include "Instruction.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>

int main(int argc, const char** argv) {
	std::ifstream rom { "rom/IBM_Logo.ch8" };
	if (!rom) {
		std::cout << "Couldn't find file\n";
		return 1;
	}

	std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(rom), {});
	std::cout << std::hex;

	SDL_Window* window;
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Chip8", 640, 320, SDL_WINDOW_OPENGL);

	SDL_Delay(3000);
	SDL_DestroyWindow(window);

	Chip8_Emu emulator {};
	while (emulator.m_PC < buffer.size()) {
		uint16_t lower_byte = static_cast<uint16_t>(buffer[emulator.m_PC]) << 8;
		emulator.m_PC +=1;
		uint16_t upper_byte = static_cast<uint16_t>(buffer[emulator.m_PC]);
		emulator.m_PC += 1;
		uint16_t instruction_bytes = upper_byte | lower_byte;
		
		Instruction instr = Instruction(instruction_bytes);
		std::cout << instr;
		switch (instr.m_type) {
			case Instruction::TYPE::CLEAR_SCREEN: {
				std::cout << "CLEAR\n";
				break;
			}
			case Instruction::TYPE::JUMP: {
				std::cout << "JUMP\n";
				break;
			}
			case Instruction::TYPE::SET_REG: {
				std::cout << "SET REG VX\n";
				break;
			}
			case Instruction::TYPE::ADD_TO_REG: {
				std::cout << "ADD VALUE TO REG VX\n";
				break;
			}
			case Instruction::TYPE::SET_I: {
				std::cout << "SET INDEX REG I\n";
				break;
			}
			case Instruction::TYPE::DRAW: {
				std::cout << "DRAW\n";
				break;
			}
			default: {
				std::cout << "UNKNOWN\n";
			}
		}
	}
    return 0;
}
