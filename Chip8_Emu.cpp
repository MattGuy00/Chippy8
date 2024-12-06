#include "Chip8_Emu.h"
#include "Instruction.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>
#include "SDL3/SDL.h"

void Chip8_Emu::draw(SDL_Renderer* renderer) {
	for (int y = 0; y < 32; ++y) {
		for (int x = 0; x < 64; ++x) {
			if (m_screen_bitmap[x][y]) {
				SDL_FRect rect; 
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				rect.x = x;
				rect.y = y;
				rect.w = 1;
				rect.h = 1;
				SDL_RenderFillRect(renderer, &rect);
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
				SDL_RenderPresent(renderer);
			} 
			
		}
	}
}

void Chip8_Emu::play(const std::string& rom) {
	SDL_Window* window;
	SDL_Renderer* renderer { nullptr };
	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer("Chip8", 64, 32, SDL_WINDOW_RESIZABLE, &window, &renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	SDL_Event event;
	bool running { true };
	while (running) {
		SDL_PollEvent(&event);
		if (event.type == SDL_EVENT_QUIT) {
			break;
		}


		read_rom_into_memory(rom);
//		std::cout << "loc:" << m_PC << ':';

		uint16_t lower_byte = static_cast<uint16_t>(m_memory[m_PC]) << 8;
		m_PC +=1;
		uint16_t upper_byte = static_cast<uint16_t>(m_memory[m_PC]);
		m_PC += 1;
		uint16_t instruction_bytes = upper_byte | lower_byte;

		Instruction instr = Instruction(instruction_bytes);
//		std::cout << std::hex << instr;
		switch (instr.m_type) {
			case Instruction::TYPE::CLEAR_SCREEN: {
				// TODO: clear bitmap 
				for (int y = 0; y < 32; ++y) {
					for (int x = 0; x < 64; ++x) {
						m_screen_bitmap[x][y] = 0;
					}
				}
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
				SDL_RenderClear(renderer);
				SDL_RenderPresent(renderer);
				break;
			}
			case Instruction::TYPE::JUMP: {
				m_PC = instr.m_NNN;
				break;
			}
			case Instruction::TYPE::SET_REG: {
				m_registers[instr.m_X] = instr.m_NN;
				break;
			}
			case Instruction::TYPE::ADD_TO_REG: {
				m_registers[instr.m_X] += instr.m_NN;
				break;
			}
			case Instruction::TYPE::SET_I: {
				m_I = instr.m_NNN;
				break;
			}
			case Instruction::TYPE::DRAW: {
				// Sprites should wrap
				uint16_t x { static_cast<uint16_t>(m_registers[instr.m_X]  % 64) };
				uint16_t y { static_cast<uint16_t>(m_registers[instr.m_Y]  % 32) };

				// Set register VF to 0
				m_registers[0xf] = 0;

				for (int height = 0; (height < instr.m_N) && (y < 32); ++height, ++y) {
					uint8_t sprite_byte { m_memory[m_I + height] };

					int x_offset {};
					for (int i = 7; (i >= 0) && (x + x_offset < 64); --i, ++x_offset)	 {
						uint8_t pixel { static_cast<uint8_t>((sprite_byte >> i) & 0x1) };
						if (m_screen_bitmap[x + x_offset][y] && pixel) {
							m_screen_bitmap[x+ x_offset][y] = 0;
							m_registers[0xf] = 1;
						} else if (pixel)  {
							m_screen_bitmap[x+ x_offset][y] = 1;
							m_registers[0xf] = 0;
						}
					}
				}
				draw(renderer);
				break;
			}
			default: {
				break;
			}

		}


	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
