
#include "cpp_lib.h"
#include <algorithm>


void convertToGray(const BMP& input, std::vector<uint8_t>& output, Size & size, unsigned int startY, unsigned int endY)
{
	const uint32_t width = static_cast<uint32_t>(input.infoHeader.width);
	const uint32_t height = static_cast<uint32_t>(input.infoHeader.height);
	
	if (size.x != width || size.y != height) {
		size.x = width;
		size.y = height;
	}
	uint32_t stride = (width * 3 + 3) & (~3);
	for (uint32_t y = startY; y < endY; ++y) {
		uint32_t bmpRowStart = (height - 1 - y) * stride;

		const size_t outRowOffset = static_cast<size_t>(y) * width;
		for (uint32_t x = 0; x < width; ++x) {
			size_t bmpPixelOffset = static_cast<size_t>(bmpRowStart) + static_cast<size_t>(x) * 3;
			uint8_t b = input.data[bmpPixelOffset + 0];
			uint8_t g = input.data[bmpPixelOffset + 1];
			uint8_t r = input.data[bmpPixelOffset + 2];
			

			float b_part = 0.114f * static_cast<float>(b);
			float g_part = 0.587f * static_cast<float>(g);
			float r_part = 0.299f * static_cast<float>(r);

				float grayf = (b_part + g_part) + r_part;
						;
			uint8_t gray = static_cast<uint8_t>(grayf);
			output[outRowOffset + x] = gray;
		}
	}

};
void gaussFilter(const std::vector<uint8_t>& input, std::vector<uint8_t>& output, Size & size, unsigned int startY, unsigned int endY)
{

	static const int kernel[5][5] = {
	{1,4,7,4,1},
	{4,16,26,16,4},
	{7,26,41,26,7},
	{4,16,26,16,4},
	{1,4,7,4,1}
	};
	const int kernelSum = 273;

	for (unsigned int y = startY; y < endY; ++y) {
		for (int x = 0; x < size.x; ++x)
		{
			unsigned int r = 0;
			for (int i = -2; i <= 2; i++)
			{
				for (int j = -2; j <= 2; j++)
				{
					int px = std::clamp<int>(x + i, 0, size.x - 1);
					int py = std::clamp<int>(y + j, 0, size.y - 1);


					unsigned int c = input[py * size.x + px];
					r += c * kernel[j + 2][i + 2];
				}
			}
			uint8_t gauss = (r / kernelSum);

			output[y * size.x + x] = gauss;
		}
	}
};
void gradient(const std::vector<uint8_t>& input, std::vector<uint8_t>& output, std::vector<float>& direction, Size size, unsigned int startY, unsigned int endY)
{

	static const float Sx[3][3] = {
		{-1,0,1},
		{-2,0,2},
		{-1,0,1}
	};
	static const float Sy[3][3] = {
		{1,2,1},
		{0,0,0},
		{-1,-2,-1}
	};

	float r;

	std::vector<float> gradX(size.x * size.y);
	std::vector<float> gradY(size.x * size.y);


	for (unsigned int y = startY; y < endY; ++y) {
		unsigned int rowOffset = y * size.x;
		for (unsigned int x = 0; x < size.x; ++x)
		{
			unsigned int index = rowOffset + x;
			gradX[index] = 0;
			gradY[index] = 0;
			for (int i = -1; i <= 1; i++)
			{
				for (int j = -1; j <= 1; j++)
				{
					int px = std::clamp<int>(x + i, 0, size.x - 1);
					int py = std::clamp<int>(y + j, 0, size.y - 1);
					unsigned int pIndex = input[py * size.x + px];
					gradX[index] += pIndex * Sx[j + 1][i + 1];
					gradY[index] += pIndex * Sy[j + 1][i + 1];

				}
			}
			float gradient = sqrt((gradX[index]) * (gradX[index])
				+ (gradY[index]) * (gradY[index]));

			// --- Calculate direction ---
			float gx = gradX[index];
			float gy = gradY[index];
			float absX = std::abs(gx);
			float absY = std::abs(gy);

			bool swap = absY > absX;
			float num = swap ? absX : absY;
			float den = swap ? absY : absX;

			float angle = 0.0f;
			if (den > 0.0001f) { // Zabezpieczenie przed dzieleniem przez zero
				float z = num / den;

				//  PRZYBLI¯ENIE: z / (1 + 0.28086 * z^2)
				angle = z / (1.0f + 0.28086f * z * z);

				// Jeœli zamieniliœmy X z Y (dla k¹tów > 45st), wynik to PI/2 - angle
				if (swap) angle = 1.570796f - angle;
			}

			// Obs³uga æwiartek (aby zachowaæ zgodnoœæ z atan2 i Twoim warunkiem < 0)
			// Jeœli Gx jest ujemne, k¹t znajduje siê w 2 lub 3 æwiartce
			if (gx < 0) {
				angle = 3.14159f - angle;
			}
			// Jeœli Gy jest ujemne (i Gx dodatnie), k¹t jest w 4 æwiartce
			else if (gy < 0) {
				angle = -angle;
			}

			// Twoja oryginalna logika mapowania do zakresu [0, PI]
			if (angle < 0)
			{
				angle += 3.14159f;
			}
			direction[index] = angle;

			gradient = std::clamp(gradient, 0.f, 255.f);
			output[index] = static_cast<uint8_t>(gradient);
		}
	}
};
void nonMaximumSuspension(const std::vector<uint8_t>& input, std::vector<uint8_t>& output, std::vector<float>& direction, Size size, unsigned int startY, unsigned int endY)
{

	for (unsigned int y = startY; y < endY ; ++y) {
		unsigned int rowOffset = y * size.x;
		for (unsigned int x = 0; x < size.x; ++x)
		{
			unsigned int index = rowOffset + x;
			unsigned int c = input[index];
			unsigned int o;
			float theta = direction[index];
			if (x == 0 || y == 0 || x == size.x - 1 || y == size.y - 1)
			{
				o = 0;
			}
			else
			{
				if ((theta >= 0.f && theta <= 0.3927f) || (theta >= 2.7489f && theta <= 3.1416f))
				{
					unsigned int befC = input[index - 1];
					unsigned int nextC = input[index + 1];
					if (befC > c)
					{
						o = 0;
					}
					else if (nextC > c)
					{
						o = 0;
					}
					else
					{
						o = c;
					}
				}
				else if ((theta > 0.3927f && theta <= 1.1771f))
				{
					unsigned int befC = input[(y + 1) * size.x + (x - 1)];
					unsigned int nextC = input[(y - 1) * size.x + (x + 1)];
					if (befC > c)
					{
						o = 0;
					}
					else if (nextC > c)
					{
						o = 0;
					}
					else
					{
						o = c;
					}
				}
				else if ((theta > 1.1771f && theta <= 1.9635f))
				{
					unsigned int befC = input[(y - 1) * size.x + x];
					unsigned int nextC = input[(y + 1) * size.x + x];
					if (befC > c)
					{
						o = 0;
					}
					else if (nextC > c)
					{
						o = 0;
					}
					else
					{
						o = c;
					}
				}
				else
				{
					unsigned int befC = input[(y - 1) * size.x + (x - 1)];
					unsigned int nextC = input[(y + 1) * size.x + (x + 1)];
					if (befC > c)
					{
						o = 0;
					}
					else if (nextC > c)
					{
						o = 0;
					}
					else
					{
						o = c;
					}
				}
			}
			output[index] = o;

		}
	}

};
void doubleThresholding(const std::vector<uint8_t>& input, std::vector<uint8_t>& output, unsigned int strongPixel, unsigned int wheakPixel, Size size, unsigned int startY, unsigned int endY) {

	for (unsigned int y = startY; y < endY; ++y) {
		unsigned int rowOffset = y * size.x;
		for (unsigned int x = 0; x < size.x; ++x)
		{
			unsigned int index = rowOffset + x;
			unsigned int c = input[index];
			unsigned int o;
			if (c >= strongPixel)
				o = 255;
			else if (c >= wheakPixel) {
				o = 100;
			}
			else
				o = 0;
			output[index] = o;
		}
	}
};
void histerezis(const std::vector<uint8_t>& input, std::vector<uint8_t>& output, Size size, unsigned int startY, unsigned int endY)
{

	for (unsigned int y = startY; y < endY; ++y) {
		unsigned int rowOffset = y * size.x;
		for (unsigned int x = 0; x < size.x; ++x)
		{
			unsigned int index = rowOffset + x;
			unsigned int c = input[index];

			if (x == 0 || x == size.x - 1 || y == 0 || y == size.y - 1)
				output[index] = 0;
			else if (c == 255)
				output[index] = 255;
			else if (c == 100) {
				bool isNeighStrong = false;

				for (int j = -1; j <= 1 && !isNeighStrong; j++) {

					for (int i = -1; i <= 1; i++) {
						if (i + x > size.x || i + x < 0)
							continue;
						unsigned int neightPixel = input[(y + j) * size.x + (x + i)];
						if (neightPixel == 255)
						{
							isNeighStrong = true;
							break;
						}
					}
				}
				if (isNeighStrong)
					output[index] = 255;
				else
					output[index] = 0;

			}
			else
				output[index] = 0;
		}
	}
};