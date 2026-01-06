#include "threads.h"

void processInThreads(
	int numThreads,
	unsigned int imageHeight,
	std::function<void(unsigned int, unsigned int)> task)
{
	int rowsPerThread = imageHeight / numThreads;
	std::vector<std::thread> threads;

	for (int i = 0; i < numThreads; ++i) {
		unsigned int startY = i * rowsPerThread;
		unsigned int endY;
		if (i == numThreads - 1) endY = imageHeight;
		else endY = (i + 1) * rowsPerThread;


		threads.emplace_back(task, startY, endY);
	}

	for (auto& t : threads)
		t.join();
};