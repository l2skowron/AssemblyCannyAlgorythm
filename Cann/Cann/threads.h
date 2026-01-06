#pragma once
#include<functional>
#include<thread>
void processInThreads(
	int numThreads,
	unsigned int imageHeight,
	std::function<void(unsigned int, unsigned int)> task);