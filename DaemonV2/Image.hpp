#if !defined(_IMAGE_HPP_)
#define _IMAGE_HPP_

#include <cstdint>
#include <vector>
#include <stdexcept>
#include "Hardware/Location.hpp"

class Image
{
private:
	std::vector<std::uint16_t> data;
	int width, height;

public:
	Image(int width, int height)
		: width(width), height(height) {
		this->data.reserve(width * height);
		this->Clear();
	}
	
	void Clear(void) noexcept {
		for (int i = 0; i < this->width * this->height; i++)
			this->data[i] = 0x0000;
	}

	int GetHeight(void) const noexcept {
		return this->height;
	}

	int GetWidth(void) const noexcept {
		return this->width;
	}

	const uint16_t* GetData(void) const noexcept { return this->data.data(); }
	void ProcessMeasurementPayload(const void* vpayload, int bits, const Location& location);

};

#endif // _IMAGE_HPP_
