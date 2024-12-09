#pragma once

#include "Bitmap.h"
#include "Window.h"
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <optional>
#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>

class Chip8_Emu {
public:
	// Fixed width and height until we add option for user
	// to change it
	static constexpr int m_window_width { 640 };
	static constexpr int m_window_height { 320 };

	void play(const std::string& rom);

	std::array<std::uint8_t, 4096> m_memory {
		// Font
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	Bitmap bitmap { m_window_width, m_window_height };
	std::vector<uint16_t> m_stack {};

	//	COMSAC LAYOUT: =>	MODERN LAYOUT:
	//	1 2 3 C				1 2 3 4
	//	4 5 6 D				Q W E R
	//	7 8 9 E				A S D F 
	//	A 0 B F				Z X C V
	std::array<bool, SDL_SCANCODE_COUNT> m_keys {};
	std::unordered_map<uint8_t, uint16_t> m_keymap {
		{ 0x0, SDL_SCANCODE_X },
		{ 0x1, SDL_SCANCODE_1 },
		{ 0x2, SDL_SCANCODE_2 },
		{ 0x3, SDL_SCANCODE_3 },
		{ 0x4, SDL_SCANCODE_Q },
		{ 0x5, SDL_SCANCODE_W },
		{ 0x6, SDL_SCANCODE_E },
		{ 0x7, SDL_SCANCODE_A },
		{ 0x8, SDL_SCANCODE_S },
		{ 0x9, SDL_SCANCODE_D },
		{ 0xa, SDL_SCANCODE_Z },
		{ 0xb, SDL_SCANCODE_C },
		{ 0xc, SDL_SCANCODE_4 },
		{ 0xd, SDL_SCANCODE_R },
		{ 0xe, SDL_SCANCODE_F },
		{ 0xf, SDL_SCANCODE_V }
	};
	uint8_t m_delay_timer {};
	uint8_t m_sound_timer {};
	int m_PC { 512 };
	int m_I {};

	Window window { "Chip8", m_window_width, m_window_height };

	std::array<uint8_t, 16> m_registers {};
private:
	void read_rom_into_memory(const std::string& rom) {
		std::ifstream rom_file { rom, std::ios::binary };	
		int rom_memory_start_pos { 511 };
		// TODO: Add error checking
		auto size { std::filesystem::file_size(rom) };
		rom_file.read(reinterpret_cast<char*>(m_memory.data()) + 512, size);

	}

	std::optional<uint16_t> get_pressed_key() {
		for (const auto& key : m_keymap) {
			if (m_keys[key.second]) {
				return key.second;	
			}
		}
		return {};
	}

};
