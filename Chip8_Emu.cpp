#include "Chip8_Emu.h"
#include "Instruction.h"
#include <iostream>

void Chip8_Emu::play(const std::string& rom) {
	read_rom_into_memory(rom);
	int count = 0;
	while (count < 10) {
		count++;
		uint16_t lower_byte = static_cast<uint16_t>(m_memory[m_PC]) << 8;
		m_PC +=1;
		uint16_t upper_byte = static_cast<uint16_t>(m_memory[m_PC]);
		m_PC += 1;
		uint16_t instruction_bytes = upper_byte | lower_byte;
		
		Instruction instr = Instruction(instruction_bytes);
		std::cout << std::hex << instr;
		switch (instr.m_type) {
			case Instruction::TYPE::CLEAR_SCREEN: {
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
				// TODO: Add drawing support with SDL
				break;
			}
			default: {
				break;
			}
		}
	}

}
