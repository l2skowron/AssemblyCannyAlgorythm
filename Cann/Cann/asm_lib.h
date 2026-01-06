#pragma once
#include <vector>
#include "GUI.h"
#include "BMP.h"

extern "C" __declspec(dllimport)
void convertToGray_asm(const BMP& input, std::vector<uint8_t>& output, Size& size, unsigned int startY, unsigned int endY);
