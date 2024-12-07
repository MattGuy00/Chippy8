#include "Chip8_Emu.h"
#include "Instruction.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <cstdint>


void Chip8_Emu::play(const std::string& rom) {
	SDL_Event event;
	bool running { true };
	read_rom_into_memory(rom);

	while (running) {
		SDL_PollEvent(&event);
		if (event.type == SDL_EVENT_QUIT) {
			break;
		}

		uint16_t lower_byte = static_cast<uint16_t>(m_memory[m_PC]) << 8;
		m_PC +=1;
		uint16_t upper_byte = static_cast<uint16_t>(m_memory[m_PC]);
		m_PC += 1;
		uint16_t instruction_bytes = upper_byte | lower_byte;

		Instruction instr = Instruction(instruction_bytes);
		std::cout << "loc:" << m_PC << ':';
		std::cout << std::hex << instr;
		switch (instr.m_type) {
			case Instruction::TYPE::CLEAR_SCREEN: {
				// TODO: clear bitmap 
				bitmap.clear_bitmap();
				window.draw_background(0, 0, 0, 255);
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
				
				// Draw to the window by xoring the bit at x, y in the bitmap 
				// with each inidivdual pixel (bit) 
				// starting at the memory location stored in m_I 
				for (int height = 0; (height < instr.m_N) && (y < 32); ++height, ++y) {
					uint8_t sprite_byte { m_memory[m_I + height] };

					int x_offset {};
					for (int i = 7; (i >= 0) && (x + x_offset < 64); --i, ++x_offset)	 {
						uint8_t pixel { static_cast<uint8_t>((sprite_byte >> i) & 0x1) };
						if (bitmap.data[x + x_offset][y] && pixel) {
							bitmap.data[x+ x_offset][y] = 0;
							m_registers[0xf] = 1;
						} else if (pixel)  {
							bitmap.data[x+ x_offset][y] = 1;
							m_registers[0xf] = 0;
						}
					}
				}
				window.draw_bitmap(bitmap);
				break;
			}
			default: {
				break;
			}
		}
	}
}
