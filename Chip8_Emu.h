#pragma once

#include "Bitmap.h"
#include "Instruction.h"
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
#include <thread>
#include <mutex>

class Chip8_Emu {
public:
	Chip8_Emu() {
		// Run thread in background that increments the timers
		m_timer_thread = std::thread(&Chip8_Emu::increment_timers, this);
	}

	~Chip8_Emu() {
		m_running = false;
		m_timer_thread.join();
	}

	void play(const std::string& rom);

private:
	static constexpr int m_memory_size { 4096 };
	static constexpr int m_window_width { 640 };
	static constexpr int m_window_height { 320 };
	static constexpr uint16_t m_rom_start_pos { 512 };

	std::thread m_timer_thread {};
	std::mutex m_timer_lock {};

	uint8_t m_delay_timer {};
	uint8_t m_sound_timer {};
	int m_PC { 512 };
	int m_I {};

	Window m_window { "Chippy8", m_window_width, m_window_height };
	Bitmap m_bitmap { m_window_width, m_window_height };
	std::vector<uint16_t> m_stack {};

	std::array<uint8_t, 16> m_registers {};
	bool m_running { true };

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

	// Fetches the next instruction, incrementing the pc by 2	
	Instruction fetch_next_instruction() {
		int upper_byte = m_memory[m_PC] << 8;
		m_PC +=1;
		int lower_byte = m_memory[m_PC];
		m_PC += 1;
		return { static_cast<uint16_t>(upper_byte | lower_byte) };
	}

	// Increments the sound and delay timers. To be used outside of main thread
	void increment_timers() {
		while (m_running) {
			m_timer_lock.lock();
			if (m_sound_timer > 0) {
				--m_sound_timer;
			}

			if (m_delay_timer > 0) {
				--m_delay_timer;
			}
			m_timer_lock.unlock();

			// Roughly 60hz
			std::this_thread::sleep_for(std::chrono::milliseconds(17));
		}
	}

	bool try_load_rom_into_memory(const std::string& rom) {
		std::ifstream rom_file { rom, std::ios::binary };	

		if (!rom_file.good()) {
			std::cerr << "Rom Error: Bad rom.\n";
			return false;
		} 

		auto size { std::filesystem::file_size(rom) };
		// If the file is too large to fit into memory we should quit
		if ((size > (m_memory_size - m_rom_start_pos))) {
			std::cerr << "Rom Error: Rom unable to fit into memory - ";
			std::cerr << "Max size: " << m_memory_size - m_rom_start_pos << ", " ;
			std::cerr << "Rom size: " << size << '\n';
			return false;
		}

		rom_file.read(reinterpret_cast<char*>(m_memory.data()) + m_rom_start_pos, size);
		return true;
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
