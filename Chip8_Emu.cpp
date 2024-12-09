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
#include <cstdlib>
#include <mutex>
#include <thread>

std::mutex timer_lock {};

static void increment_timers(uint8_t& sound_timer, uint8_t& delay_timer, bool& running) {
	while (running) {
		timer_lock.lock();
		if (sound_timer > 0) {
			--sound_timer;
		}

		if (delay_timer > 0) {
			--delay_timer;
		}
		timer_lock.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(17));
	}
}

void Chip8_Emu::play(const std::string& rom) {
	bool running { true };
	read_rom_into_memory(rom);

	std::thread timer(increment_timers, std::ref(m_sound_timer), std::ref(m_delay_timer), std::ref(running));
	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_EVENT_QUIT: {
					running = false;
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

		uint16_t lower_byte = static_cast<uint16_t>(m_memory[m_PC]) << 8;
		m_PC +=1;
		uint16_t upper_byte = static_cast<uint16_t>(m_memory[m_PC]);
		m_PC += 1;
		uint16_t instruction_bytes = upper_byte | lower_byte;

		Instruction instr = Instruction(instruction_bytes);
		std::cout << "loc:" << m_PC << ':';
		std::cout << std::hex << instr << '\n';
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
				uint16_t x { static_cast<uint16_t>(m_registers[instr.m_X] % m_window_width) };
				uint16_t y { static_cast<uint16_t>(m_registers[instr.m_Y] % m_window_height) };

				// Set register VF to 0
				m_registers[0xf] = 0;
				
				// Draw to the window by xoring the bit at x, y in the bitmap 
				// with each inidivdual pixel (bit) 
				// starting at the memory location stored in m_I 
				for (int height = 0; (height < instr.m_N) && (y < m_window_height); ++height, ++y) {
					uint8_t sprite_byte { m_memory[m_I + height] };

					int x_offset {};
					for (int i = 7; (i >= 0) && (x + x_offset < m_window_width); --i, ++x_offset) {
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
			case Instruction::TYPE::SKIP_X_EQ_NN: {
				if (m_registers[instr.m_X] == instr.m_NN) {
					m_PC += 2;
				}
				break;
			}
			case Instruction::TYPE::SKIP_X_NE_NN: {
				if (m_registers[instr.m_X] != instr.m_NN) {
					m_PC += 2;
				}
				break;
			}
			case Instruction::TYPE::SKIP_X_Y_EQ: {
				if (m_registers[instr.m_X] == m_registers[instr.m_Y]) {
					m_PC += 2;
				}
				break;
			}
			case Instruction::TYPE::SKIP_X_Y_NE: {
				if (m_registers[instr.m_X] != m_registers[instr.m_Y]) {
					m_PC += 2;
				}
				break;
			}
			case Instruction::TYPE::CALL: {
				m_stack.push_back(m_PC);
				m_PC = instr.m_NNN;
				break;
			}
			case Instruction::TYPE::RETURN: {
				m_PC = m_stack.back();
				m_stack.pop_back();
				break;
			}
			case Instruction::TYPE::SET: {
				m_registers[instr.m_X] = m_registers[instr.m_Y];
				break;
			}
			case Instruction::TYPE::OR: {
				m_registers[instr.m_X] |= m_registers[instr.m_Y];
				break;
			}
			case Instruction::TYPE::AND: {
				m_registers[instr.m_X] &= m_registers[instr.m_Y];
				break;
			}
			case Instruction::TYPE::XOR: {
				m_registers[instr.m_X] ^= m_registers[instr.m_Y];
				break;
			}
			case Instruction::TYPE::ADD: {
				// Store in an int to check for 8 bit overflow
				int value { m_registers[instr.m_X] + m_registers[instr.m_Y] };

				// Store the sum and let it overflow 
				m_registers[instr.m_X] = value;
				// Set carry flag on overflow
				m_registers[0xf] = value > 255;
				break;
			}
			case Instruction::TYPE::SUBTRACT_X_Y: {
				uint8_t prev_reg_value { m_registers[instr.m_X] };
				m_registers[instr.m_X] -= m_registers[instr.m_Y];
				m_registers[0xf] = prev_reg_value > m_registers[instr.m_Y];
				break;
			}
			case Instruction::TYPE::SUBTRACT_Y_X: {
				uint8_t prev_reg_value { m_registers[instr.m_X] };
				m_registers[instr.m_X] = m_registers[instr.m_Y] - m_registers[instr.m_X];
				m_registers[0xf] = m_registers[instr.m_Y] > prev_reg_value;
				break;
			}
			case Instruction::TYPE::SHIFT_LEFT: {
				int shifted_bit { (m_registers[instr.m_X] >> 7) & 0x1 };
				m_registers[instr.m_X] <<= 1;
				m_registers[0xf] = shifted_bit;
				break;
			}
			case Instruction::TYPE::SHIFT_RIGHT: {
				int shifted_bit { m_registers[instr.m_X] & 0x1 };
				m_registers[instr.m_X] >>= 1;
				m_registers[0xf] = shifted_bit;
				break;
			}
			case Instruction::TYPE::BCD_CONVERSION: {
				// Too lazy to put in loop
				uint8_t value { m_registers[instr.m_X] };
				uint8_t hundreds { static_cast<uint8_t>((value / 100) % 10) };
				uint8_t tens { static_cast<uint8_t>((value / 10) % 10) };
				uint8_t ones { static_cast<uint8_t>(value % 10) };
				m_memory[m_I] = hundreds;
				m_memory[m_I + 1] = tens;
				m_memory[m_I + 2] = ones;
				break;
			}
			case Instruction::TYPE::STORE: {
				for (int i = 0; i <= instr.m_X; ++i) {
					m_memory[m_I + i] = m_registers[i];
				}
				break;
			}
			case Instruction::TYPE::LOAD: {
				for (int i = 0; i <= instr.m_X; ++i) {
					m_registers[i] = m_memory[m_I + i];
				}
				break;
			}
			case Instruction::TYPE::ADD_TO_INDEX: {
				m_I += m_registers[instr.m_X];
				break;
			}
			case Instruction::TYPE::SET_DELAY_TIMER: {
				// TODO: wrap in mutex
				m_delay_timer = m_registers[instr.m_X];
				break;
			}
			case Instruction::TYPE::SET_X_DELAY_TIMER: {
				timer_lock.lock();
				m_registers[instr.m_X] = m_delay_timer;
				timer_lock.unlock();
				break;
			}
			case Instruction::TYPE::SET_SOUND_TIMER: {
				timer_lock.lock();
				m_sound_timer = m_registers[instr.m_X];
				timer_lock.unlock();
				break;
			}
			case Instruction::TYPE::SKIP_KEY_PRESSED: {
				uint16_t key { m_keymap[m_registers[instr.m_X]] };
				if (m_keys[key]) {
					m_PC += 2;
				}
				break;
			}
			case Instruction::TYPE::SKIP_KEY_NOT_PRESSED: {
				uint16_t key { m_keymap[m_registers[instr.m_X]] };
				if (!m_keys[key]) {
					m_PC += 2;
				}
				break;
			}
			case Instruction::TYPE::GET_FONT_CHAR: {
				m_I = m_registers[instr.m_X];
				break;
			}
			case Instruction::TYPE::RANDOM: {
				m_registers[instr.m_X] = (rand() % 256) & instr.m_NN;
				break;
			}
			default: {
				// Exit when we encounter unknown instruction
				std::cout << "Missing implementation: " << instr << '\n';
				return;
			}
		}
	}
	timer.join();
}
