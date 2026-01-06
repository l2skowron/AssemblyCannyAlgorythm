#pragma once
#include<cstdint>

#ifdef CANASMDLL_EXPORTS
#define CANASMDLL_API __declspec(dllexport)
#else
#define CANASMDLL_API __declspec(dllimport)
#endif

extern "C" {
	CANASMDLL_API void convertToGrayAsm(const uint8_t* input, uint8_t* output, Size& size, unsigned int startY, unsigned int endY);
	CANASMDLL_API void GaussFilterAsm(const uint8_t* input, uint8_t* output, Size& size, unsigned int startY, unsigned int endY);
	CANASMDLL_API void GradientAsm(const uint8_t* input, uint8_t* output, float* direction, Size& size, unsigned int startY, unsigned int endY);
	CANASMDLL_API void NonMaximumSuspensionAsm(const uint8_t* input, uint8_t* output, const float* direction, Size& size, unsigned int startY, unsigned int endY);
	CANASMDLL_API void DoubleThresholdingAsm(const uint8_t* input, uint8_t* output, unsigned int strongPixel, unsigned int wheakPixel, Size& size, unsigned int startY, unsigned int endY);
	CANASMDLL_API void HisterezisAsm(const uint8_t* input, uint8_t* output,  Size& size, unsigned int startY, unsigned int endY);
}

