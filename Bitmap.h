#pragma once

#include <cstdint>
#include <vector>
struct Bitmap {
	Bitmap(int w, int h) {
		width = w;
		height = h;
		data = std::vector<std::vector<uint8_t>>(w, std::vector<uint8_t>(h, 0));
	}

	void clear_bitmap() {
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				data[x][y] = 0;
			}
		}
	}

	int width {};
	int height {};
	std::vector<std::vector<uint8_t>> data {};
};
