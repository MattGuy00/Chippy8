#pragma once

#include <iostream>
#include <cstdint>
#include <string_view>

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
		m_type_nibble = (instruction_bytes >> 12) & mask;
		m_X = (instruction_bytes >> 8) & mask;
		m_Y = (instruction_bytes >> 4) & mask;
		m_N = instruction_bytes & mask;
		m_NN = (m_Y << 4) | m_N;
		m_NNN = (m_X << 8) | m_NN;
		switch (m_type_nibble) {
			case 0x0: {
				if (m_Y == 0xe && m_N == 0x0) {
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
	
	uint16_t m_type_nibble {};
	uint16_t m_X {};
	uint16_t m_Y {};
	uint16_t m_N {};
	uint16_t m_NN {};
	uint16_t m_NNN {};
	TYPE m_type { Instruction::TYPE::EMPTY };
};

static constexpr std::string_view getInstructionName(Instruction::TYPE type) {
	using enum Instruction::TYPE;
	switch (type) {
		case CLEAR_SCREEN: return "CLEAR_SCREEN";
		case SET_REG: return "SET_REG" ;
		case SET_I: return "SET_I";
		case JUMP: return "JUMP";
		case DRAW: return "DRAW";
		case ADD_TO_REG: return "ADD_TO_REG";
		case EMPTY: return "UNKNOWN";
	}
}

std::ostream& operator<<(std::ostream& out, const Instruction& instruction) {
	// Print hex version of instruction
	out << static_cast<uint16_t>(instruction.m_type_nibble) 
		<< static_cast<uint16_t>(instruction.m_X)
		<< static_cast<uint16_t>(instruction.m_Y)
		<< static_cast<uint16_t>(instruction.m_N)
		<< ":	" ;

	// Now print instruction	
	out << getInstructionName(instruction.m_type) << ' ';
	using enum Instruction::TYPE;
	switch (instruction.m_type) {
		case CLEAR_SCREEN:
			out << '\n';
			break;
		case JUMP:
			out << instruction.m_NNN << '\n';
			break;
		case SET_REG:
		case ADD_TO_REG:
			out << instruction.m_NN << ", V" << instruction.m_X << '\n';
			break;
		case SET_I:
			out << instruction.m_NNN << '\n';
			break;
		case DRAW:
			out << instruction.m_X << ' ' << instruction.m_Y << ' ' << instruction.m_N << '\n';
			break;
		default:
			out << '\n';
			break;
	}
		
	return out;
}
