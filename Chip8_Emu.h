#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <string>
#include <iostream>
#include <vector>

class Chip8_Emu {
public:
	void play(const std::string& rom);

	std::array<std::uint8_t, 4096> m_memory {
		// Font
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80,  // F
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	};
	int m_display_width { 64 };
	int m_display_height { 32 };

	std::vector<uint16_t> m_stack {};

	int m_delay_timer {};
	int m_sound_timer {};
	int m_PC { 511 };
	int m_I {};

	std::array<uint16_t, 16> m_registers {};
private:
	void read_rom_into_memory(const std::string& rom) {
		std::ifstream rom_file { rom, std::ios::binary };	
		int rom_memory_start_pos { 511 };
		// TODO: Add error checking
		auto size { std::filesystem::file_size(rom) };
		rom_file.read(reinterpret_cast<char*>(m_memory.data()) + 511, size);

	}
};
