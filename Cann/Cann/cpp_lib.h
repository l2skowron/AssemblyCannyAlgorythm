#pragma once
#include<vector>
#include "GUI.h"
#include "BMP.h"
#include <algorithm>

#ifdef Canny_EXPORTS
#define CANNY_API __declspec(dllexport)
#else
#define CANNY_API __declspec(dllimport)
#endif


#define M_PI 3.14159265358979323846;
CANNY_API void convertToGray(const BMP& input, std::vector<uint8_t>& output, Size& size, unsigned int startY, unsigned int endY);
CANNY_API void gaussFilter(const std::vector<uint8_t>& input, std::vector<uint8_t>& output, Size& size, unsigned int startY, unsigned int endY);
CANNY_API void gradient(const std::vector<uint8_t>& input, std::vector<uint8_t>& output, std::vector<float>& direction, Size size, unsigned int startY, unsigned int endY);
CANNY_API void nonMaximumSuspension(const std::vector<uint8_t>& input, std::vector<uint8_t>& output, std::vector<float>& direction, Size size, unsigned int startY, unsigned int endY);
CANNY_API void doubleThresholding(const std::vector<uint8_t>& input, std::vector<uint8_t>& output, unsigned int strongPixel, unsigned int wheakPixel, Size size, unsigned int startY, unsigned int endY);
CANNY_API void histerezis(const std::vector<uint8_t>& input, std::vector<uint8_t>& output, Size size, unsigned int startY, unsigned int endY);
