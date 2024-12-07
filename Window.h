#pragma once

#include "Bitmap.h"
#include "SDL3/SDL.h"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <cstdint>
#include <string_view>

class Window {
public:
	Window(std::string_view name, int width, int height) {
		m_width = width;
		m_height = height;
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
		SDL_CreateWindowAndRenderer(name.data(), width, height, SDL_WINDOW_OPENGL, &window, &m_renderer);
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
		int w;
		int h;
		SDL_GetWindowSize(window, &w, &h);
		float scale_w = w / 64;
		float scale_h = h / 32;
		SDL_FRect pixel { 0, 0, scale_w, scale_h };
		// Using floats instead of ints to avoid messy static casting
		for (float y = 0; y < bitmap.height; ++y) {
			for (float x = 0; x < bitmap.width; ++x) {
				// Only draw pixel that are on
				if (bitmap.data[x][y]) {
					pixel.x = x * scale_w;
					pixel.y = y * scale_h; 
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

	int m_width {};
	int m_height {};
};
