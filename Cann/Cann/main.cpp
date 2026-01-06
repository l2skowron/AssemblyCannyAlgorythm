#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <string>
#include<Windows.h>
#include <commdlg.h>
#include "GUI.h"
#include "FileDialog.h"
#include <vector>
#include <thread>
#include <mutex>
#include<algorithm>
#include<cmath>
#include<regex>
#include "cpp_lib.h"
#include "threads.h"
#include "BMP.h"
#include <cmath>
#include "../CAN_ASMDll/CAN_ASMDll.h"
#include<fstream>

int main ()
{
	GuiContext gui;
	
	int no_of_threads = 0;
	sf::Vector2u windowSize = gui.window.getSize();
	unsigned int width = windowSize.x, height = windowSize.y;
	gui.background.emplace(gui.backgroundTexture);	

	
		
		bool imageloaded = false;
		while (gui.window.isOpen())
		{
			while (const std::optional event = gui.window.pollEvent())
			{
				if (event->is<sf::Event::Closed>())
					gui.window.close();
				if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
					if (mouse->button == sf::Mouse::Button::Left) {
						sf::Vector2i pixelPos = sf::Mouse::getPosition(gui.window); 
						sf::Vector2f mousePosF = gui.window.mapPixelToCoords(pixelPos); 


						if (gui.libcpp->isClicked(mousePosF)) {
							gui.libcpp->setActive(true);
							gui.libasm->setActive (false);
						}
						else if (gui.libasm->isClicked(mousePosF)) {
							gui.libcpp->setActive(false);
							gui.libasm->setActive(true);
						}
						else if (gui.thread_1->isClicked(mousePosF)) {
							no_of_threads = 1;
							gui.thread_1->setActive(true);
							gui.thread_2->setActive(false);
							gui.thread_4->setActive(false);
							gui.thread_8->setActive(false);
							gui.thread_16->setActive(false);
							gui.thread_32->setActive(false);
							gui.thread_64->setActive(false);					
						}
						else if (gui.thread_2->isClicked(mousePosF)) {
							no_of_threads = 2;
							gui.thread_1->setActive(false);
							gui.thread_2->setActive(true);
							gui.thread_4->setActive(false);
							gui.thread_8->setActive(false);
							gui.thread_16->setActive(false);
							gui.thread_32->setActive(false);
							gui.thread_64->setActive(false);
						}
						else if (gui.thread_4->isClicked(mousePosF)) {
							no_of_threads = 4;
							gui.thread_1->setActive(false);
							gui.thread_2->setActive(false);
							gui.thread_4->setActive(true);
							gui.thread_8->setActive(false);
							gui.thread_16->setActive(false);
							gui.thread_32->setActive(false);
							gui.thread_64->setActive(false);
						}
						else if (gui.thread_8->isClicked(mousePosF)) {
							no_of_threads = 8;
							gui.thread_1->setActive(false);
							gui.thread_2->setActive(false);
							gui.thread_4->setActive(false);
							gui.thread_8->setActive(true);
							gui.thread_16->setActive(false);
							gui.thread_32->setActive(false);
							gui.thread_64->setActive(false);
						}
						else if(gui.thread_16->isClicked(mousePosF)) {
							no_of_threads = 16;
							gui.thread_1->setActive(false);
							gui.thread_2->setActive(false);
							gui.thread_4->setActive(false);
							gui.thread_8->setActive(false);
							gui.thread_16->setActive(true);
							gui.thread_32->setActive(false);
							gui.thread_64->setActive(false);
						}
						else if (gui.thread_32->isClicked(mousePosF)) {
							no_of_threads = 32;
							gui.thread_1->setActive(false);
							gui.thread_2->setActive(false);
							gui.thread_4->setActive(false);
							gui.thread_8->setActive(false);
							gui.thread_16->setActive(false);
							gui.thread_32->setActive(true);
							gui.thread_64->setActive(false);
						}
						else if (gui.thread_64->isClicked(mousePosF)) {
							no_of_threads = 64;
							gui.thread_1->setActive(false);
							gui.thread_2->setActive(false);
							gui.thread_4->setActive(false);
							gui.thread_8->setActive(false);
							gui.thread_16->setActive(false);
							gui.thread_32->setActive(false);
							gui.thread_64->setActive(true);
						}
						else if (gui.presentation->isClicked(mousePosF))
						{
							if(gui.presentation->getActive()==false)
								gui.presentation->setActive(true);
							else
								gui.presentation->setActive(false);
						}
						else if (gui.startbutton->isClicked(mousePosF))
						{
							if ((no_of_threads == 0) && ((gui.libcpp->getActive() == false) && (gui.libasm->getActive() == false) && imageloaded == false))
							{
								gui.message->setString("Choose DLL, no. of threads and an image.", width, height);
							}
							else if ((no_of_threads == 0) && ((gui.libcpp->getActive() == false) && (gui.libasm->getActive() == false)) && (imageloaded == true))
							{
								gui.message->setString("Choose DLL and no. of threads.", width, height);
							}
							else if ((no_of_threads == 0) && ((gui.libcpp->getActive() == true) || (gui.libasm->getActive() == true)) && (imageloaded == true))
							{
								gui.message->setString("Choose no. of threads.", width, height);

							}
							else if ((no_of_threads > 0) && ((gui.libcpp->getActive() == false) && (gui.libasm->getActive() == false)) && (imageloaded == true))
							{
								gui.message->setString("Choose DLL.", width, height);

							}
							else if ((no_of_threads == 0) && ((gui.libcpp->getActive() == true) || (gui.libasm->getActive() == true)) && (imageloaded == false))
							{
								gui.message->setString("Choose no. of threads and an image.", width, height);
							}
							else if ((no_of_threads > 0) && ((gui.libcpp->getActive() == false) && (gui.libasm->getActive() == false)) && (imageloaded == false))
							{
								gui.message->setString("Choose DLL and an image.", width, height);
							}
							else if ((no_of_threads > 0) && ((gui.libcpp->getActive() == true) || (gui.libasm->getActive() == true)) && imageloaded == false)
							{
								gui.message->setString("Select an image.", width, height);
							}
							else if ((imageloaded == true) && (gui.presentation->getActive() == true) && (gui.libcpp->getActive() == true) && (gui.libasm->getActive() == false) && (no_of_threads > 0))
							{

								gui.message->setString("Simulating...", width, height);
								{
									std::this_thread::sleep_for(std::chrono::seconds(1));
									gui.message->setString("Processing Grayscale...", width, height);


									std::thread processingThread([&]() {
										sf::Vector2u size2u = gui.Image.getSize();
										Size size;
										size.x = size2u.x;
										size.y = size2u.y;
										std::vector<uint8_t> grayVector(size.x * size.y);
										std::vector<uint8_t> gaussVector(size.x * size.y);
										std::vector<uint8_t> gradientVector(size.x * size.y);
										std::vector<uint8_t> suspensionVector(size.x * size.y);
										std::vector<uint8_t> ThresholdVector(size.x * size.y);
										std::vector<uint8_t> FinalVector(size.x * size.y);


										sf::Image grayImage(sf::Vector2u(size.x, size.y));
										sf::Image gaussImage(sf::Vector2u(size.x, size.y));
										sf::Image gradientImage(sf::Vector2u(size.x, size.y));
										sf::Image suspensionImage(sf::Vector2u(size.x, size.y));
										sf::Image ThresholdImage(sf::Vector2u(size.x, size.y));
										sf::Image FinalImage(sf::Vector2u(size.x, size.y));
										std::vector<float> direction(size.x * size.y);
										float maxVal = 0;
										auto start = std::chrono::high_resolution_clock::now();


										std::ofstream plik("direction_cpp.txt");
										std::ofstream plik2("double_cpp.txt");
										// Etap 1 – grayscale
										processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
											convertToGray(gui.bmpImage, grayVector, size, startY, endY);
											});
										processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
											vectorToImage(grayVector, grayImage, size, startY, endY);
											});
										{
											std::lock_guard<std::mutex> lock(imageMutex);
											processedImage = grayImage;
										}
										currentStage = ProcessingStage::Grayscale;
										stageUpdated = true;
											 //Etap 2 – Gaussian blur
											processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
												gaussFilter(grayVector, gaussVector, size, startY, endY);
												});
											processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
												vectorToImage(gaussVector, gaussImage, size, startY, endY);
												});
											{
												std::lock_guard<std::mutex> lock(imageMutex);
												processedImage = gaussImage;
											}
											currentStage = ProcessingStage::Gaussian;
											stageUpdated = true;
										//Etap 3 - Obliczanie gradientu
										processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
											gradient(gaussVector, gradientVector, direction, size, startY, endY);
											});
																				
										processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
											vectorToImage(gradientVector, gradientImage, size, startY, endY);
											});
										{
											std::lock_guard<std::mutex> lock(imageMutex);
											processedImage = gradientImage;
										}
										currentStage = ProcessingStage::Gradient;
										stageUpdated = true;
										//if (plik.is_open())
										//{
										//for (unsigned int i = 0; i < gradientVector.size(); i++)
										//	{
										//	// wypisanie wartoœci direction do pliku
										//	
										//		plik << direction[i] << "\n";
										//	}
										//}
										plik.close();
										//Etap 4 - Supresja lokalnych nie maksimów
										processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
											nonMaximumSuspension(gradientVector, suspensionVector, direction, size, startY, endY);
											});
										processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
											vectorToImage(suspensionVector, suspensionImage, size, startY, endY);
											});
										{
											std::lock_guard<std::mutex> lock(imageMutex);
											processedImage = suspensionImage;
										}
										currentStage = ProcessingStage::NonMaximalSuspention;
										stageUpdated = true;
										//if (plik2.is_open())
										//{
										//	for (unsigned int i = 0; i < suspensionVector.size(); i++)
										//	{
										//		// wypisanie wartoœci direction do pliku

										//		plik2 << static_cast<int>(suspensionVector[i]) << "\n";
										//	}
										//}
										//plik2.close();
										//Etap 5 - Podwójne progowanie
										float maxGradient = *std::max_element(gradientVector.begin(), gradientVector.end());
										unsigned int strongPixel = 0.2 * maxGradient;
										unsigned int weakPixel = 0.4 * strongPixel;
										processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
											doubleThresholding(suspensionVector, ThresholdVector, strongPixel, weakPixel, size, startY, endY);
											});
										processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
											vectorToImage(ThresholdVector, ThresholdImage, size, startY, endY);
											});
										{
											std::lock_guard<std::mutex> lock(imageMutex);
											processedImage = ThresholdImage;
										}
										currentStage = ProcessingStage::DoubleThreshold;
										stageUpdated = true;
										//Etap 6 - Histereza
										processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
											histerezis(ThresholdVector, FinalVector, size, startY, endY);
											});
										processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
											vectorToImage(FinalVector, FinalImage, size, startY, endY);
											});

										{
											std::lock_guard<std::mutex> lock(imageMutex);
											processedImage = FinalImage;
										}
										currentStage = ProcessingStage::Hysteresis;
										auto end = std::chrono::high_resolution_clock::now();
										stageUpdated = true;
										auto cpp_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
										gui.processingTimeMs = cpp_time / 1000.0;
										});
									processingThread.detach();
								}
							}

							else if ((gui.libcpp->getActive() == true) && (imageloaded == true) && (gui.presentation->getActive() == false) && (gui.libasm->getActive() == false) && (no_of_threads > 0))
							{

								gui.message->setString("Processing...", width, height);
								std::thread processingThread([&]() {
									sf::Vector2u size2u = gui.Image.getSize();
									Size size;
									size.x = size2u.x;
									size.y = size2u.y;
									std::vector<uint8_t> grayVector(size.x * size.y);
									std::vector<uint8_t> gaussVector(size.x * size.y);
									std::vector<uint8_t> gradientVector(size.x * size.y);
									std::vector<uint8_t> suspensionVector(size.x * size.y);
									std::vector<uint8_t> ThresholdVector(size.x * size.y);
									std::vector<uint8_t> FinalVector(size.x * size.y);

									sf::Image FinalImage(sf::Vector2u(size.x, size.y));
									std::vector<float> direction(size.x * size.y);
									float maxVal = 0;
									auto start = std::chrono::high_resolution_clock::now();
									// Etap 1 – grayscale
									processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
										convertToGray(gui.bmpImage, grayVector, size, startY, endY);
										});
									currentStage = ProcessingStage::Grayscale;
									stageUpdated = true;
									// Etap 2 – Gaussian blur
									processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
										gaussFilter(grayVector, gaussVector, size, startY, endY);
										});
									currentStage = ProcessingStage::Gaussian;
									stageUpdated = true;
									//Etap 3 - Obliczanie gradientu
									processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
										gradient(gaussVector, gradientVector, direction, size, startY, endY);
										});
									currentStage = ProcessingStage::Gradient;
									stageUpdated = true;
									//Etap 4 - Supresja lokalnych nie maksimów
									processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
										nonMaximumSuspension(gradientVector, suspensionVector, direction, size, startY, endY);
										});
									currentStage = ProcessingStage::NonMaximalSuspention;
									stageUpdated = true;
									//Etap 5 - Podwójne progowanie
									float maxGradient = *std::max_element(gradientVector.begin(), gradientVector.end());
									unsigned int strongPixel = 0.2 * maxGradient;
									unsigned int weakPixel = 0.4 * strongPixel;
									processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
										doubleThresholding(suspensionVector, ThresholdVector, strongPixel, weakPixel, size, startY, endY);
										});
									currentStage = ProcessingStage::DoubleThreshold;
									stageUpdated = true;
									//Etap 6 - Histereza
									processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
										histerezis(ThresholdVector, FinalVector, size, startY, endY);
										});
									auto end = std::chrono::high_resolution_clock::now();
									processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
										vectorToImage(FinalVector, FinalImage, size, startY, endY);
										});
									{
										std::lock_guard<std::mutex> lock(imageMutex);
										processedImage = FinalImage;
									}
									currentStage = ProcessingStage::Hysteresis;
									stageUpdated = true;
									auto cpp_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
									gui.processingTimeMs = cpp_time / 1000.0;
									});
								processingThread.detach();
							}

							else if ((gui.libasm->getActive() == true) && (imageloaded == true) && (gui.presentation->getActive() == true) && (no_of_threads > 0))
							{
								gui.message->setString("Processing...", width, height);
								std::thread processingThread([&]() {
									sf::Vector2u size2u = gui.Image.getSize();
									Size size;
									size.x = size2u.x;
									size.y = size2u.y;
									std::vector<uint8_t> grayVectorASM(size.x * size.y);
									std::vector<uint8_t> gaussVectorASM(size.x * size.y);
									std::vector<uint8_t> gradientVectorASM(size.x * size.y);
									std::vector<uint8_t> suspensionVectorASM(size.x* size.y);
									std::vector<uint8_t> ThresholdVectorASM(size.x* size.y);
									std::vector<uint8_t> FinalVectorASM(size.x* size.y);
									sf::Image grayImageAsm(sf::Vector2u(size.x, size.y));
									sf::Image gaussImageAsm(sf::Vector2u(size.x, size.y));
									sf::Image gradientImageAsm(sf::Vector2u(size.x, size.y));
									sf::Image suspensionImageAsm(sf::Vector2u(size.x, size.y));
									sf::Image ThresholdImageAsm(sf::Vector2u(size.x, size.y));
									sf::Image FinalImageAsm(sf::Vector2u(size.x, size.y));
									std::vector<float> directionAsm(size.x * size.y);
									

									std::ofstream plik("direction_values_asm.txt");
									std::ofstream plik_double("double_asm.txt");

									processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
										convertToGrayAsm(gui.bmpImage.data.data(), grayVectorASM.data(), size, startY, endY);
										});
									processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
										vectorToImage(grayVectorASM, grayImageAsm, size, startY, endY);
										});
									{
										std::lock_guard<std::mutex> lock(imageMutex);
										processedImage = grayImageAsm;
									}
									currentStage = ProcessingStage::Grayscale;
									stageUpdated = true;

									processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
										GaussFilterAsm(grayVectorASM.data(), gaussVectorASM.data(), size, startY, endY);
										});
									processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
										vectorToImage(gaussVectorASM, gaussImageAsm, size, startY, endY);
										});
									{
										std::lock_guard<std::mutex> lock(imageMutex);
										processedImage = gaussImageAsm;
									}
									currentStage = ProcessingStage::Gaussian;
									stageUpdated = true;

									processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
										GradientAsm(gaussVectorASM.data(), gradientVectorASM.data(), directionAsm.data(), size, startY, endY);
										});
									processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
										vectorToImage(gradientVectorASM, gradientImageAsm, size, startY, endY);
										});
									{
										std::lock_guard<std::mutex> lock(imageMutex);
										processedImage = gradientImageAsm;
									}
									currentStage = ProcessingStage::Gradient;
									stageUpdated = true;
									//if (plik.is_open())
									//{
									//for (unsigned int i = 0; i < gradientVectorASM.size(); i++)
									//	{
									//	// wypisanie wartoœci direction do pliku
									//	
									//		plik << directionAsm[i] << "\n";
									//	}
									//}
									//plik.close();
									processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
										NonMaximumSuspensionAsm(gradientVectorASM.data(), suspensionVectorASM.data(), directionAsm.data(), size, startY, endY);
										});
									processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
										vectorToImage(suspensionVectorASM, suspensionImageAsm, size, startY, endY);
										});
									{
										std::lock_guard<std::mutex> lock(imageMutex);
										processedImage = suspensionImageAsm;
									}
									currentStage = ProcessingStage::NonMaximalSuspention;
									stageUpdated = true;
									float maxGradient = *std::max_element(gradientVectorASM.begin(), gradientVectorASM.end());
									unsigned int strongPixel = 0.2 * maxGradient;
									unsigned int weakPixel = 0.4 * strongPixel;
									processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
										DoubleThresholdingAsm(suspensionVectorASM.data(), ThresholdVectorASM.data(),strongPixel, weakPixel, size, startY, endY);
										});
									processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
										vectorToImage(ThresholdVectorASM, ThresholdImageAsm, size, startY, endY);
										});
									{
										std::lock_guard<std::mutex> lock(imageMutex);
										processedImage = ThresholdImageAsm;
									}
									currentStage = ProcessingStage::DoubleThreshold;
									stageUpdated = true;
									processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
										HisterezisAsm(ThresholdVectorASM.data(), FinalVectorASM.data(), size, startY, endY);
										});
									processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
										vectorToImage(FinalVectorASM, FinalImageAsm, size, startY, endY);
										});
									{
										std::lock_guard<std::mutex> lock(imageMutex);
										processedImage = FinalImageAsm;				
									}
									currentStage = ProcessingStage::Hysteresis;
									stageUpdated = true;
									//if (plik_double.is_open())
									//{
									//	for (unsigned int i = 0; i < suspensionVectorASM.size(); i++)
									//	{
									//		// wypisanie wartoœci direction do pliku
									//		plik_double << static_cast<int>(suspensionVectorASM[i]) << "\n";
									//	
									//	}
									//}
									//plik_double.close();
									});

								processingThread.detach();
							}
							else if ((gui.libasm->getActive() == true) && (imageloaded == true) && (gui.presentation->getActive() == false) && (no_of_threads > 0))
							{
							}
						}
						else if (gui.select_image->isClicked(mousePosF))
						{
							std::string imagePath = openFileDialog();
							if (!imagePath.empty())
								if (!gui.Image.loadFromFile(imagePath)) {
									std::cerr << "Nie uda³o siê za³adowaæ obrazka!\n";
								}
								else if (!gui.image_be.loadFromFile(imagePath))
								{
									std::cerr << "Nie uda³o siê za³adowaæ obrazka!\n";
								}
								else {
									imageloaded = true;
									sfImageToBMP(gui.Image, gui.bmpImage);
									gui.be_sprite.emplace(gui.image_be);
									sf::Vector2f scale;
									scale = rescall_Image(gui.image_be, width / 2.6, height / 2.f);
									gui.be_sprite->setScale(scale);
									sf::Vector2u pos = gui.image_be.getSize();
									if (pos.x > pos.y)
									{
										gui.be_sprite->setPosition(sf::Vector2f(width / 3.f - 350, height / 2.f - 300));
									}
									else
									{
										gui.be_sprite->setPosition(sf::Vector2f(width / 3.f - 200, height / 2.f - 300));
									}
								
						}
						}
						else if (gui.select_other_image->isClicked(mousePosF))
						{
							std::string imagePath = openFileDialog();
							if (!imagePath.empty())
								if (!gui.Image.loadFromFile(imagePath)) {
									std::cerr << "Nie uda³o siê za³adowaæ obrazka!\n";
								}
								else if (!gui.image_be.loadFromFile(imagePath))
								{
									std::cerr << "Nie uda³o siê za³adowaæ obrazka!\n";
								}
								else {
									sfImageToBMP(gui.Image, gui.bmpImage);
									
									imageloaded = true;
									gui.be_sprite.emplace(gui.image_be);
									sf::Vector2f scale = rescall_Image(gui.image_be, width / 2.6, height / 2.f);
									gui.be_sprite->setScale(scale);
									sf::Vector2u pos = gui.image_be.getSize();
									if (pos.x > pos.y)
									{
										gui.be_sprite->setPosition(sf::Vector2f(width / 3.f - 350, height / 2.f - 300));
									}
									else
									{
										gui.be_sprite->setPosition(sf::Vector2f(width / 3.f - 200, height / 2.f - 300));
									}
								}
						}
						else if (gui.save_button->isClicked(mousePosF))
						{
							if (currentStage != ProcessingStage::None)
							{
								std::lock_guard<std::mutex> lock(imageMutex);
								if (processedImage.getSize().x > 0 && processedImage.getSize().y > 0)
								{
									std::string savePath = saveFileDialog();
									if (processedImage.saveToFile(savePath))
										std::cout << "Image saved to " << savePath << "\n";
									else
										std::cout << "Failed to save image!\n";
								}
							}
						}
					}
				}
			}


						gui.window.clear();
						if (gui.background)
						gui.window.draw(*gui.background);
						gui.rect1->draw(gui.window);
						gui.rect2->draw(gui.window);
						gui.title->draw(gui.window);
						gui.lib->draw(gui.window);
						gui.libcpp->draw(gui.window);
						gui.libasm->draw(gui.window);
						gui.thread->draw(gui.window);
						gui.thread_1->draw(gui.window);
						gui.thread_2->draw(gui.window);
						gui.thread_4->draw(gui.window);
						gui.thread_8->draw(gui.window);
						gui.thread_16->draw(gui.window);
						gui.thread_32->draw(gui.window);
						gui.thread_64->draw(gui.window);
						gui.startbutton->draw(gui.window);
						gui.before_image->draw(gui.window);
						gui.after_image->draw(gui.window);
						gui.presentation->draw(gui.window);
						if (!imageloaded)
							gui.select_image->draw(gui.window);
						gui.message->draw(gui.window);
						if (imageloaded && gui.be_sprite)
						{
							gui.window.draw(*gui.be_sprite);
							gui.select_other_image->draw(gui.window);
						}
						static sf::Texture tex;
						static sf::Sprite sprite(tex);

						// aktualizuj teksturê tylko jeœli przetwarzanie zakoñczy³o etap
						if (stageUpdated) {
							std::lock_guard<std::mutex> lock(imageMutex);
							if (processedImage.getSize().x > 0 && processedImage.getSize().y > 0) {
								if (!tex.loadFromImage(processedImage)) {
									std::cout << "Failed to load texture from processedImage!\n";
								}
								else {
									sprite.setTexture(tex, true);
									sf::Vector2f scale = rescall_Image(tex, width / 2.6f, height / 2.f);
									sprite.setScale(scale);

									sf::Vector2u pos = gui.image_be.getSize();
									if (pos.x > pos.y)
										sprite.setPosition(sf::Vector2f(width / 3.f * 2.f - 200, height / 2.f - 300));
									else
										sprite.setPosition(sf::Vector2f(width / 3.f * 2.f, height / 2.f - 300));
								}
							}
							stageUpdated = false;
						}
						if (currentStage != ProcessingStage::None) {
							gui.window.draw(sprite);
							gui.save_button->draw(gui.window);
							std::string stageName;
							switch (currentStage) {
							case ProcessingStage::Grayscale: stageName = "Gaussian Blur..."; break;
							case ProcessingStage::Gaussian: stageName = "Gradient..."; break;
							case ProcessingStage::Gradient: stageName = "NonMaximalSuspention..."; break;
							case ProcessingStage::NonMaximalSuspention: stageName = "DoubleThreshold..."; break;
							case ProcessingStage::DoubleThreshold: stageName = "Hysterezis..."; break;
							case ProcessingStage::Hysteresis: stageName = "Done."; break;						
							default: break;
							}

							gui.message->setString("Procesing stage: " + stageName, width, height);

								
						}
						
						if (currentStage == ProcessingStage::Hysteresis) {
							gui.timeMessage->setString("Work time: " + std::to_string(gui.processingTimeMs.load()) + " s", width / 2.f+25, height - 200.f);
							gui.timeMessage->draw(gui.window);
						}
						gui.window.display();
					}
				
			
		

};