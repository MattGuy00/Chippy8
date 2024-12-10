#pragma once

#include <cstdint>
#include <vector>

struct Bitmap {
	int width {};
	int height {};
	std::vector<std::vector<uint8_t>> data = std::vector<std::vector<uint8_t>>(width, std::vector<uint8_t>(height, 0));

	void clear_bitmap() {
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				data[x][y] = 0;
			}
		}
	}

};
