#pragma once

#include "Bitmap.h"
#include "SDL3/SDL.h"
#include <cstdint>
#include <string_view>

class Window {
public:
	Window(std::string_view name, int width, int height) {
		SDL_Init(SDL_INIT_VIDEO);
		SDL_CreateWindowAndRenderer(name.data(), width, height, SDL_WINDOW_RESIZABLE, &window, &m_renderer);
		SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
		SDL_RenderClear(m_renderer);
		SDL_RenderPresent(m_renderer);
	}

	~Window() {
		SDL_DestroyRenderer(m_renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	void draw_background(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
		SDL_RenderClear(m_renderer);
		SDL_RenderPresent(m_renderer);
	}

	void draw_bitmap(const Bitmap& bitmap) {
		SDL_FRect pixel { 0, 0, 1, 1 };
		// Using floats instead of ints to avoid messy static casting
		for (float y = 0; y < bitmap.height; ++y) {
			for (float x = 0; x < bitmap.width; ++x) {
				// Only draw pixel that are on
				if (bitmap.data[x][y]) {
					pixel.x = x;
					pixel.y = y;
					SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
					SDL_RenderFillRect(m_renderer, &pixel);
					SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
					SDL_RenderPresent(m_renderer);
				} 
			}
		}
	}

	SDL_Window* window {};
	SDL_Renderer* m_renderer {};
};
