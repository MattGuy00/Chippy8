#include "Chip8_Emu.h"
#include "Instruction.h"

#include <SDL3/SDL.h>
#include <cstdint>
#include <optional>
#include <mutex>

void Chip8_Emu::play(const std::string& rom) {
	if (!try_load_rom_into_memory(rom)) {
		return;
	}

	while (m_running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_EVENT_QUIT: {
					m_running = false;
					break;
				}
				case SDL_EVENT_KEY_DOWN: {
					m_keys[event.key.scancode] = true;
					break;
				}
				case SDL_EVENT_KEY_UP: {
					m_keys[event.key.scancode] = false;
					break;
				}
			}
		}

		Instruction instruction = fetch_next_instruction();
		std::cout << std::hex << instruction << '\n';
		switch (instruction.m_type) {
			case Instruction::TYPE::CLEAR_SCREEN: {
				m_bitmap.clear_bitmap();
				m_window.draw_background(0, 0, 0, 255);
				break;
			}
			case Instruction::TYPE::JUMP: {
				m_PC = instruction.m_NNN;
				break;
			}
			case Instruction::TYPE::JUMP_OFFSET: {
				m_PC = instruction.m_NNN + m_registers[0];
				break;
			}
			case Instruction::TYPE::SET_REG: {
				m_registers[instruction.m_X] = instruction.m_NN;
				break;
			}
			case Instruction::TYPE::ADD_TO_REG: {
				m_registers[instruction.m_X] += instruction.m_NN;
				break;
			}
			case Instruction::TYPE::SET_I: {
				m_I = instruction.m_NNN;
				break;
			}
			case Instruction::TYPE::DRAW: {
				// Sprites should wrap
				uint16_t x { static_cast<uint16_t>(m_registers[instruction.m_X] % m_window_width) };
				uint16_t y { static_cast<uint16_t>(m_registers[instruction.m_Y] % m_window_height) };

				// Set register VF to 0
				m_registers[0xf] = 0;
				
				// Draw to the window by xoring the bit at x, y in the bitmap 
				// with each inidivdual pixel (bit) 
				// starting at the memory location stored in m_I 
				for (int height = 0; (height < instruction.m_N) && (y < m_window_height); ++height, ++y) {
					uint8_t sprite_byte { m_memory[m_I + height] };

					int x_offset {};
					for (int i = 7; (i >= 0) && (x + x_offset < m_window_width); --i, ++x_offset) {
						uint8_t pixel { static_cast<uint8_t>((sprite_byte >> i) & 0x1) };
						if (m_bitmap.data[x + x_offset][y] && pixel) {
							m_bitmap.data[x+ x_offset][y] = 0;
							m_registers[0xf] = 1;
						} else if (pixel)  {
							m_bitmap.data[x+ x_offset][y] = 1;
							m_registers[0xf] = 0;
						}
					}
				}
				m_window.draw_bitmap(m_bitmap);
				break;
			}
			case Instruction::TYPE::SKIP_X_EQ_NN: {
				if (m_registers[instruction.m_X] == instruction.m_NN) {
					m_PC += 2;
				}
				break;
			}
			case Instruction::TYPE::SKIP_X_NE_NN: {
				if (m_registers[instruction.m_X] != instruction.m_NN) {
					m_PC += 2;
				}
				break;
			}
			case Instruction::TYPE::SKIP_X_Y_EQ: {
				if (m_registers[instruction.m_X] == m_registers[instruction.m_Y]) {
					m_PC += 2;
				}
				break;
			}
			case Instruction::TYPE::SKIP_X_Y_NE: {
				if (m_registers[instruction.m_X] != m_registers[instruction.m_Y]) {
					m_PC += 2;
				}
				break;
			}
			case Instruction::TYPE::CALL: {
				m_stack.push_back(m_PC);
				m_PC = instruction.m_NNN;
				break;
			}
			case Instruction::TYPE::RETURN: {
				m_PC = m_stack.back();
				m_stack.pop_back();
				break;
			}
			case Instruction::TYPE::SET: {
				m_registers[instruction.m_X] = m_registers[instruction.m_Y];
				break;
			}
			case Instruction::TYPE::OR: {
				m_registers[instruction.m_X] |= m_registers[instruction.m_Y];
				break;
			}
			case Instruction::TYPE::AND: {
				m_registers[instruction.m_X] &= m_registers[instruction.m_Y];
				break;
			}
			case Instruction::TYPE::XOR: {
				m_registers[instruction.m_X] ^= m_registers[instruction.m_Y];
				break;
			}
			case Instruction::TYPE::ADD: {
				// Store in an int to check for 8 bit overflow
				int value { m_registers[instruction.m_X] + m_registers[instruction.m_Y] };

				// Store the sum and let it overflow 
				m_registers[instruction.m_X] = value;
				// Set carry flag on overflow
				m_registers[0xf] = value > 255;
				break;
			}
			case Instruction::TYPE::SUBTRACT_X_Y: {
				uint8_t prev_reg_value { m_registers[instruction.m_X] };
				m_registers[instruction.m_X] -= m_registers[instruction.m_Y];
				m_registers[0xf] = prev_reg_value > m_registers[instruction.m_Y];
				break;
			}
			case Instruction::TYPE::SUBTRACT_Y_X: {
				uint8_t prev_reg_value { m_registers[instruction.m_X] };
				m_registers[instruction.m_X] = m_registers[instruction.m_Y] - m_registers[instruction.m_X];
				m_registers[0xf] = m_registers[instruction.m_Y] > prev_reg_value;
				break;
			}
			case Instruction::TYPE::SHIFT_LEFT: {
				int shifted_bit { (m_registers[instruction.m_X] >> 7) & 0x1 };
				m_registers[instruction.m_X] <<= 1;
				m_registers[0xf] = shifted_bit;
				break;
			}
			case Instruction::TYPE::SHIFT_RIGHT: {
				int shifted_bit { m_registers[instruction.m_X] & 0x1 };
				m_registers[instruction.m_X] >>= 1;
				m_registers[0xf] = shifted_bit;
				break;
			}
			case Instruction::TYPE::BCD_CONVERSION: {
				// Too lazy to put in loop
				uint8_t value { m_registers[instruction.m_X] };
				uint8_t hundreds { static_cast<uint8_t>((value / 100) % 10) };
				uint8_t tens { static_cast<uint8_t>((value / 10) % 10) };
				uint8_t ones { static_cast<uint8_t>(value % 10) };
				m_memory[m_I] = hundreds;
				m_memory[m_I + 1] = tens;
				m_memory[m_I + 2] = ones;
				break;
			}
			case Instruction::TYPE::STORE: {
				for (int i = 0; i <= instruction.m_X; ++i) {
					m_memory[m_I + i] = m_registers[i];
				}
				break;
			}
			case Instruction::TYPE::LOAD: {
				for (int i = 0; i <= instruction.m_X; ++i) {
					m_registers[i] = m_memory[m_I + i];
				}
				break;
			}
			case Instruction::TYPE::ADD_TO_INDEX: {
				m_I += m_registers[instruction.m_X];
				break;
			}
			case Instruction::TYPE::SET_DELAY_TIMER: {
				m_timer_lock.lock();
				m_delay_timer = m_registers[instruction.m_X];
				m_timer_lock.unlock();
				break;
			}
			case Instruction::TYPE::SET_X_DELAY_TIMER: {
				m_timer_lock.lock();
				m_registers[instruction.m_X] = m_delay_timer;
				m_timer_lock.unlock();
				break;
			}
			case Instruction::TYPE::SET_SOUND_TIMER: {
				m_timer_lock.lock();
				m_sound_timer = m_registers[instruction.m_X];
				m_timer_lock.unlock();
				break;
			}
			case Instruction::TYPE::SKIP_KEY_PRESSED: {
				uint16_t key { m_keymap[m_registers[instruction.m_X]] };
				if (m_keys[key]) {
					m_PC += 2;
				}
				break;
			}
			case Instruction::TYPE::SKIP_KEY_NOT_PRESSED: {
				uint16_t key { m_keymap[m_registers[instruction.m_X]] };
				if (!m_keys[key]) {
					m_PC += 2;
				}
				break;
			}
			case Instruction::TYPE::GET_FONT_CHAR: {
				m_I = m_registers[instruction.m_X];
				break;
			}
			case Instruction::TYPE::RANDOM: {
				m_registers[instruction.m_X] = (rand() % 256) & instruction.m_NN;
				break;
			}
			case Instruction::TYPE::WAIT_FOR_KEYPRESS: {
				auto opt_scancode { get_pressed_key() };
				if (opt_scancode){
					m_registers[instruction.m_X] = opt_scancode.value();
				} else {
					m_PC -= 2;
				}
				break;
			}
			default: {
				// Exit when we encounter unknown instruction
				std::cout << "Missing implementation: " << instruction << '\n';
				return;
			}
		}
	}
}
