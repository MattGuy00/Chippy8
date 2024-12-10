#pragma once

#include <iostream>
#include <cstdint>
#include <string_view>

class Instruction {
public:
	enum class TYPE {
		CLEAR_SCREEN,
		JUMP,
		JUMP_OFFSET,
		SET_REG,
		ADD_TO_REG,
		SET_I,
		DRAW,
		SKIP_X_EQ_NN,
		SKIP_X_NE_NN,
		SKIP_X_Y_EQ,
		SKIP_X_Y_NE,
		SKIP_KEY_PRESSED,
		SKIP_KEY_NOT_PRESSED,
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
		WAIT_FOR_KEYPRESS,
		GET_FONT_CHAR,
		RANDOM,
		UNKNOWN
	};

	Instruction(uint16_t instruction_bytes) 
		:	m_type_nibble { static_cast<uint16_t>((instruction_bytes >> 12) & 0xf) },
			m_X { static_cast<uint16_t>((instruction_bytes >> 8) & 0xf) },
			m_Y { static_cast<uint16_t>((instruction_bytes >> 4) & 0xf)},
			m_N { static_cast<uint16_t>(instruction_bytes & 0xf) },
			m_NN { static_cast<uint16_t>((m_Y << 4) | m_N) },
			m_NNN { static_cast<uint16_t>((m_X << 8) | m_NN) },
			m_type { get_instruction_type() }
	{
	}

	// Keep this order or bad things will happen	
	uint16_t m_type_nibble {};
	uint16_t m_X {};
	uint16_t m_Y {};
	uint16_t m_N {};
	uint16_t m_NN {};
	uint16_t m_NNN {};
	TYPE m_type { Instruction::TYPE::UNKNOWN };

	std::string_view get_instruction_name() const {
		using enum Instruction::TYPE;
		switch (m_type) {
			case CLEAR_SCREEN: return "CLEAR_SCREEN";
			case SET_REG: return "SET_REG" ;
			case SET_I: return "SET_I";
			case JUMP: return "JUMP";
			case JUMP_OFFSET: return "JUMP_OFFSET";
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
			case UNKNOWN: return "UNKNOWN";
		}
	}

	friend std::ostream& operator<<(std::ostream& out, const Instruction& instruction) {
		// Print hex version of instruction
		out << static_cast<uint16_t>(instruction.m_type_nibble) 
			<< static_cast<uint16_t>(instruction.m_X)
			<< static_cast<uint16_t>(instruction.m_Y)
			<< static_cast<uint16_t>(instruction.m_N)
			<< ":	" ;

		// TODO: Move this fetch/decode loop so we can print actual values
		out << instruction.get_instruction_name() << ' ';
		using enum Instruction::TYPE;
		switch (instruction.m_type) {
			case CLEAR_SCREEN:
				out << '\n';
				break;
			case JUMP:
				out << instruction.m_NNN;
				break;
			case JUMP_OFFSET:
				out << "V0 + " << instruction.m_NNN;
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
			case UNKNOWN:
				out << '\n';
				break;
		}

		return out;
	}
private:
	TYPE get_instruction_type() {
		using enum Instruction::TYPE;
		switch (m_type_nibble) {
			case 0x0:
				if (m_NNN == 0x0e0) {
					return CLEAR_SCREEN;
				} else if (m_NNN == 0x0ee) {
					return RETURN;
				}
			case 0x1: return JUMP;
			case 0x2: return CALL;
			case 0x3: return SKIP_X_EQ_NN;
			case 0x4: return SKIP_X_NE_NN;
			case 0x5: return SKIP_X_Y_EQ;
			case 0x6: return SET_REG;
			case 0x7: return ADD_TO_REG;
			case 0x8:
				switch (m_N) {
					case 0x0: return SET;
					case 0x1: return OR;
					case 0x2: return AND;
					case 0x3: return XOR;
					case 0x4: return ADD;
					case 0x5: return SUBTRACT_X_Y;
					case 0x7: return SUBTRACT_Y_X;
					case 0x6: return SHIFT_RIGHT;
					case 0xe: return SHIFT_LEFT;
				}
			case 0x9: return SKIP_X_Y_NE;
			case 0xa: return SET_I;
			case 0xb: return JUMP_OFFSET;
			case 0xc: return RANDOM;
			case 0xd: return DRAW;
			case 0xe:
				switch (m_NN) {
					case 0x9e: return SKIP_KEY_PRESSED;
					case 0xa1: return SKIP_KEY_NOT_PRESSED;
				}
			case 0xf:
				switch (m_NN) {
					case 0x33: return BCD_CONVERSION;
					case 0x55: return STORE;
					case 0x65: return LOAD;
					case 0x1e: return ADD_TO_INDEX;
					case 0x15: return SET_DELAY_TIMER;
					case 0x07: return SET_X_DELAY_TIMER;
					case 0x18: return SET_SOUND_TIMER;
					case 0x29: return GET_FONT_CHAR;
					case 0x0a: return WAIT_FOR_KEYPRESS;
				}
			default: return UNKNOWN;
		}
	}
};

