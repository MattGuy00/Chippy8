#pragma once

#include <iostream>
#include <array>
#include <cstdint>
class Instruction {
public:
	enum class TYPE {
		CLEAR_SCREEN = 0x0,
		JUMP = 0x1,
		SET_REG = 0x6,
		ADD_TO_REG = 0x7,
		SET_I = 0xa,
		DRAW = 0xd,
		EMPTY,
	};

	Instruction(uint16_t instruction_bytes) {
		uint8_t mask { 0xf };
		m_nibbles[0] = (instruction_bytes >> 12) & mask;
		m_nibbles[1] = (instruction_bytes >> 8) & mask;
		m_nibbles[2] = (instruction_bytes >> 4) & mask;
		m_nibbles[3] = instruction_bytes & mask;
		switch (m_nibbles[0]) {
			case 0x0: {
				if (m_nibbles[2] == 0xe && m_nibbles[3] == 0x0) {
					m_type = TYPE::CLEAR_SCREEN;
				}
				break;
			}
			case 0x1: {
				m_type = TYPE::JUMP;
				break;
			}
			case 0x6: {
				m_type = TYPE::SET_REG;
				break;
			}
			case 0x7: {
				m_type = TYPE::ADD_TO_REG;
				break;
			}
			case 0xa: {
				m_type = TYPE::SET_I;
				break;
			}
			case 0xd: {
				m_type = TYPE::DRAW;
				break;
			}
			default: {
				m_type = TYPE::EMPTY;
				break;
			}
		}
	}

	friend std::ostream& operator<<(std::ostream& out, const Instruction& instruction);
	
	std::array<uint8_t, 4> m_nibbles {};
	TYPE m_type { Instruction::TYPE::EMPTY };
};

std::ostream& operator<<(std::ostream& out, const Instruction& instruction) {
	out << static_cast<uint16_t>(instruction.m_nibbles[0]) 
		<< static_cast<uint16_t>(instruction.m_nibbles[1])
		<< static_cast<uint16_t>(instruction.m_nibbles[2])
		<< static_cast<uint16_t>(instruction.m_nibbles[3])
		<< ":	" ;
		
	return out;
}
