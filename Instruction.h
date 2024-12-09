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
		SKIP_X_EQ_NN,
		SKIP_X_NE_NN,
		SKIP_X_Y_EQ,
		SKIP_X_Y_NE,
		CALL,
		RETURN,
		SET,
		OR,
		AND,
		XOR,
		ADD,
		SUBTRACT_X_Y,
		SUBTRACT_Y_X,
		SHIFT_LEFT,
		SHIFT_RIGHT,
		BCD_CONVERSION,
		STORE,
		LOAD,
		ADD_TO_INDEX,
		SET_DELAY_TIMER,
		SET_SOUND_TIMER,
		SET_X_DELAY_TIMER,
		SKIP_KEY_PRESSED,
		SKIP_KEY_NOT_PRESSED,
		WAIT_FOR_KEYPRESS,
		GET_FONT_CHAR,
		RANDOM,
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
				if (m_NNN == 0x0e0) {
					m_type = TYPE::CLEAR_SCREEN;
				} else if (m_NNN == 0x0ee) {
					m_type = TYPE::RETURN;
				}
				break;
			}
			case 0x1: {
				m_type = TYPE::JUMP;
				break;
			}
			case 0x2: {
				m_type = TYPE::CALL;
				break;
			}
			case 0x3: {
				m_type = TYPE::SKIP_X_EQ_NN;
				break;
			}
			case 0x4: {
				m_type = TYPE::SKIP_X_NE_NN;
				break;
			}
			case 0x5: {
				m_type = TYPE::SKIP_X_Y_EQ;
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
			case 0x8: {
				switch (m_N) {
					case 0x0: m_type = TYPE::SET; break;
					case 0x1: m_type = TYPE::OR; break;
					case 0x2: m_type = TYPE::AND; break;
					case 0x3: m_type = TYPE::XOR; break;
					case 0x4: m_type = TYPE::ADD; break;
					case 0x5: m_type = TYPE::SUBTRACT_X_Y; break;
					case 0x6: m_type = TYPE::SHIFT_RIGHT; break;
					case 0x7: m_type = TYPE::SUBTRACT_Y_X; break;
					case 0xe: m_type = TYPE::SHIFT_LEFT; break;
				}

				break;
			}
			case 0x9: {
				m_type = TYPE::SKIP_X_Y_NE;
				break;
			}
			case 0xa: {
				m_type = TYPE::SET_I;
				break;
			}
			case 0xc: m_type = TYPE::RANDOM; break;
			case 0xd: {
				m_type = TYPE::DRAW;
				break;
			}
			case 0xe: {
				switch (m_NN) {
					case 0x9e: m_type = TYPE::SKIP_KEY_PRESSED; break;
					case 0xa1: m_type = TYPE::SKIP_KEY_NOT_PRESSED; break;
				}
				break;
			}
			case 0xf: {
				switch (m_NN) {
					case 0x33: m_type = TYPE::BCD_CONVERSION; break;
					case 0x55: m_type = TYPE::STORE; break;
					case 0x65: m_type = TYPE::LOAD; break;
					case 0x1e: m_type = TYPE::ADD_TO_INDEX; break;
					case 0x15: m_type = TYPE::SET_DELAY_TIMER; break;
					case 0x07: m_type = TYPE::SET_X_DELAY_TIMER; break;
					case 0x18: m_type = TYPE::SET_SOUND_TIMER; break;
					case 0x29: m_type = TYPE::GET_FONT_CHAR; break;
					case 0x0a: m_type = TYPE::WAIT_FOR_KEYPRESS; break;
				}
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
		case SKIP_X_EQ_NN: return "SKIP_X_EQ_NN";
		case SKIP_X_NE_NN: return "SKIP_X_NE_NN";
		case SKIP_X_Y_EQ: return "SKIP_X_Y_EQ";
		case SKIP_X_Y_NE: return "SKIP_X_Y_NE";
		case CALL: return "CALL";
		case RETURN: return "RETURN";
		case SET: return "SET";
		case OR: return "OR";
		case AND: return "AND";
		case XOR: return "XOR";
		case ADD: return "ADD";
		case SUBTRACT_X_Y: return "SUBTRACT_X_Y";
		case SUBTRACT_Y_X: return "SUBTRACT_Y_X";
		case SHIFT_LEFT: return "SHIFT_LEFT";
		case SHIFT_RIGHT: return "SHIFT_RIGHT";
		case BCD_CONVERSION: return "BCD_CONVERSION";
		case STORE: return "STORE";
		case LOAD: return "LOAD";
		case ADD_TO_INDEX: return "ADD_TO_INDEX";
		case SET_DELAY_TIMER: return "SET_DELAY_TIMER";
		case SET_X_DELAY_TIMER: return "SET_X_DELAY_TIMER";
		case SET_SOUND_TIMER: return "SET_SOUND_TIMER";
		case SKIP_KEY_PRESSED: return "SKIP_KEY_PRESSED";
		case SKIP_KEY_NOT_PRESSED: return "SKIP_KEY_NOT_PRESSED";
		case GET_FONT_CHAR: return "GET_FONT_CHAR";
		case RANDOM: return "RANDOM";
		case WAIT_FOR_KEYPRESS: return "WAIT_FOR_KEYPRESS";
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

	// TODO: Move this fetch/decode loop so we can print actual values
	out << getInstructionName(instruction.m_type) << ' ';
	using enum Instruction::TYPE;
	switch (instruction.m_type) {
		case CLEAR_SCREEN:
			out << '\n';
			break;
		case JUMP:
			out << instruction.m_NNN;
			break;
		case SET_REG:
		case ADD_TO_REG:
			out << instruction.m_NN << ", V" << instruction.m_X;
			break;
		case SET_I:
			out << instruction.m_NNN;
			break;
		case DRAW:
			out << instruction.m_X << ' ' << instruction.m_Y << ' ' << instruction.m_N;
			break;
		case SKIP_X_EQ_NN:
		case SKIP_X_NE_NN:
		case SKIP_X_Y_EQ:
		case SKIP_X_Y_NE:
			out << instruction.m_X << ", " << instruction.m_Y;
			break;
		case CALL:
			out << instruction.m_NNN;
			break;
		case SET:
		case ADD:
		case SUBTRACT_Y_X:
		case SUBTRACT_X_Y:
		case SHIFT_RIGHT:
		case SHIFT_LEFT:
			out << 'V' << instruction.m_Y << ", V" << instruction.m_X;
			break;
		case OR:
		case AND:
		case XOR:
			out << 'V' << instruction.m_X << " V" << instruction.m_Y;
			break;
		case BCD_CONVERSION:
		case SET_DELAY_TIMER:
		case SET_SOUND_TIMER:
		case SET_X_DELAY_TIMER:
		case SKIP_KEY_PRESSED:
		case SKIP_KEY_NOT_PRESSED:
		case GET_FONT_CHAR:
		case ADD_TO_INDEX:
			out << 'V' << instruction.m_X; 
			break;
		case STORE:
		case LOAD:
			out << "V0..V" << instruction.m_X;
			break;
		case RANDOM: 
			out << instruction.m_NN;
			break;
		case RETURN:
		case WAIT_FOR_KEYPRESS:
		case EMPTY:
			out << '\n';
			break;
	}
		
	return out;
}
