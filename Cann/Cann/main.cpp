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
#include <functional>
#include <mutex>
#include<algorithm>
#include<cmath>
#include<chrono>
#define M_PI 3.14159265358979323846;
enum class ProcessingStage {
	None,
	Grayscale,
	Gaussian,
	Gradient,
	NonMaximalSuspention,
	DoubleThreshold,
	Hysteresis,
	Done
};
struct weakPixel {
	int x;
	int y;
};
std::atomic<ProcessingStage> currentStage = ProcessingStage::None;
std::atomic<bool> stageUpdated = false;
std::mutex imageMutex;
sf::Image processedImage;
void convertToGray(const sf::Image& input, sf::Image& output, sf::Vector2u& size, unsigned int startY, unsigned int endY)
{
	for (unsigned int y = startY; y < endY; ++y) {
		for (int x = 0; x < size.x; ++x)
		{
			sf::Color color = input.getPixel(sf::Vector2u(x, y));
			uint8_t gray = static_cast<uint8_t>(0.299*color.r+0.587*color.g+0.114*color.b);
				output.setPixel(sf::Vector2u(x, y), sf::Color(gray, gray, gray));
		}
	}
};
void gaussFilter(const sf::Image& input, sf::Image& output, sf::Vector2u& size, unsigned int startY, unsigned int endY)
{
	static const float kernel[5][5] = {
	{1,4,7,4,1},
	{4,16,26,16,4},
	{7,26,41,26,7},
	{4,16,26,16,4},
	{1,4,7,4,1}
	};
	const float kernelSum = 273.0f;

	for (unsigned int y = startY; y < endY; ++y) {
		for (int x = 0; x < size.x; ++x)
		{
			float r = 0;
			for (int i = -2; i <= 2; i++)
			{
				for (int j = -2; j <= 2; j++)
				{
					int px = std::clamp<int>(x + i, 0, size.x - 1);
					int py = std::clamp<int>(y + j, 0, size.y - 1);


					sf::Color c = input.getPixel(sf::Vector2u(px, py));
					r += c.r * kernel[j+2][i+2];
				}
			}
			uint8_t gauss = static_cast<uint8_t>(r / kernelSum);

			output.setPixel(sf::Vector2u(x, y), sf::Color(gauss, gauss, gauss));
		}
	}
}
void gradient(const sf::Image& input, sf::Image& output,std::vector<float>& direction, sf::Vector2u size, unsigned int startY, unsigned int endY)
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

	std::vector<float> gradX(size.x*size.y);
	std::vector<float> gradY(size.x*size.y);


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
					sf::Color c = input.getPixel(sf::Vector2u(px, py));
					gradX[index] += c.r* Sx[i + 1][j + 1];
					gradY[index] += c.r * Sy[i + 1][j + 1];

				}
			}
			float gradient = sqrt((gradX[index]) * (gradX[index]) 
				+ (gradY[index] )* (gradY[index]));
			direction[index] = atan2(gradY[index], gradX[index]);
			if (direction[index] < 0)
			{
				direction[index] += M_PI;
			}
			gradient = std::clamp(gradient, 0.f, 255.f);	
			output.setPixel(sf::Vector2u(x, y), sf::Color(gradient, gradient, gradient));
			
		}	
	}
}
void nonMaximumSuspension(const sf::Image& input, sf::Image& output,std::vector<float>& direction, sf::Vector2u& size,float& maxVal, unsigned int startY, unsigned int endY)
{
	maxVal = 0;
	for (unsigned int y = startY; y < endY-1; ++y) {
		unsigned int rowOffset = y * size.x;
		for (unsigned int x = 0; x < size.x; ++x)
		{
			unsigned int index = rowOffset + x;
			sf::Color c = input.getPixel(sf::Vector2u(x, y));
			float o;
			float theta = direction[index];
			if (x == 0 || y == 0 || x == size.x - 1 || y == size.y - 1)
			{
				o = 0;
			}
			else
			{
				if ((theta >= 0 && theta <= 0.3927) || (theta >= 2.7489 && theta <= 3.1416))
				{
					sf::Color befC = input.getPixel(sf::Vector2u(x - 1, y));
					sf::Color nextC = input.getPixel(sf::Vector2u(x + 1, y));
					if (befC.r > c.r)
					{
						o = 0;
					}
					else if (nextC.r > c.r)
					{
						o = 0;
					}
					else
					{
						o = c.r;
					}
				}
				else if ((theta >= 0.3927 && theta < 1.1771))
				{
					sf::Color befC = input.getPixel(sf::Vector2u(x - 1, y + 1));
					sf::Color nextC = input.getPixel(sf::Vector2u(x + 1, y - 1));
					if (befC.r > c.r)
					{
						o = 0;
					}
					else if (nextC.r > c.r)
					{
						o = 0;
					}
					else
					{
						o = c.r;
					}
				}
				else if ((theta >= 1.1781 && theta < 1.9635))
				{
					sf::Color befC = input.getPixel(sf::Vector2u(x, y - 1));
					sf::Color nextC = input.getPixel(sf::Vector2u(x, y + 1));
					if (befC.r > c.r)
					{
						o = 0;
					}
					else if (nextC.r > c.r)
					{
						o = 0;
					}
					else
					{
						o = c.r;
					}
				}
				else
				{
					sf::Color befC = input.getPixel(sf::Vector2u(x - 1, y - 1));
					sf::Color nextC = input.getPixel(sf::Vector2u(x + 1, y + 1));
					if (befC.r>c.r)
					{
						o = 0;
					}
					else if (nextC.r>c.r)
					{
						o = 0;
					}
					else
					{
						o = c.r;
					}
				}
			}
			output.setPixel(sf::Vector2u(x, y), sf::Color(o,o,o));
			if (o > maxVal)
			{
				maxVal = o;
			}
		}
	}

}
void doubleThresholding(const sf::Image& input,sf::Image& output,std::vector<weakPixel> weakPixels,float strongPixel,float wheakPixel, sf::Vector2u& size,unsigned int startY,unsigned int endY) {
	for ( int y = startY; y < endY; ++y) {
		for (int x = 0; x < size.x; ++x)
		{
			sf::Color c = input.getPixel(sf::Vector2u(x, y));
			float o;
			if (c.r > strongPixel)
				o = 255;
			else if (c.r<strongPixel && c.r> wheakPixel){
				o = 100;
			weakPixels.push_back(weakPixel{ x,y });
			}
			else
				o = 0;
			output.setPixel(sf::Vector2u(x, y), sf::Color(o, o, o));
		}
	}
}
void histerezis(const sf::Image& input, sf::Image& output,sf::Vector2u& size,std::vector<weakPixel>& weakPixels,float strongPixel, unsigned int startY, unsigned int endY)
{ 
	for (int x = 0; x<weakPixels.size(); ++x) {
		if(weakPixels[x].x==0 || weakPixels[x].x==size.x-1 || weakPixels[x].y== 0||weakPixels[x].y==size.y-1)
			output.setPixel(sf::Vector2u(weakPixels[x].x, weakPixels[x].y), sf::Color(0, 0, 0));
		else {
			bool isNeighStrong=false;
		
				for (int i = -1; i <= 1 && !isNeighStrong; i++) {
					for (int j = -1; j <= 1; j++) {
						sf::Color neightPixel = input.getPixel(sf::Vector2u(weakPixels[x].x + i, weakPixels[x].y + j));
						if (neightPixel.r > strongPixel)
						{
							isNeighStrong = true;
							break;

						}

					}
				
				}
				if(isNeighStrong)
					output.setPixel(sf::Vector2u(weakPixels[x].x, weakPixels[x].y), sf::Color(255, 255, 255));
				else
					output.setPixel(sf::Vector2u(weakPixels[x].x, weakPixels[x].y), sf::Color(0, 0, 0));
		}
		
	}
}
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
		if (i == numThreads-1) endY = imageHeight;
		else endY = (i + 1) * rowsPerThread;


		threads.emplace_back(task, startY, endY);
	}

	for (auto& t : threads)
		t.join();
};

int main()
{
	int no_of_threads = 0;
	sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "SFML Window",sf::Style::Titlebar|sf::Style::Close);
	sf::Vector2u windowSize = window.getSize();
	unsigned int width = windowSize.x, height = windowSize.y;
	sf::Texture texture;
	std::atomic<long long> processingTimeMs{ 0 };
	std::filesystem::path path = "background.png";
	try {
		texture.loadFromFile(path);
	}
	catch (const sf::Exception& e) {
		std::cerr << "B³¹d ³adowania tekstury: " << e.what() << std::endl;
		return -1;
	}
	sf::Sprite background(texture);
	background.setScale(sf::Vector2f(1, 1));
	background.setPosition({ 0,0 });
		Rect rect1(width - 300, height - 220, { 250,110 }, sf::Color::Transparent, sf::Color::Black, 2);
		Rect rect2(246, height - 220, { 3,110 }, sf::Color::Transparent, sf::Color::Black, 2);
	auto font= sf::Font{};
	
	if (!font.openFromFile("ARLRDBD.ttf"))
	{
		std::cerr << "Couldn't load font\n";
		return -1;
	}
		std::string titl = "Application of the Canny edge detection algorithm.";
		Text title(font, titl, 50, sf::Vector2f(width / 6.f, 30.f), sf::Color::Black, sf::Text::Bold);
		Text lib(font, "Choose DDL", 30, { 25.f,150.f }, sf::Color::Black, sf::Text::Regular);
	
		Button libcpp("C++",font,{ 20.f, 200.f }, {200.f, 50.f});
		Button libasm("Asm",font,{ 20.f, 260.f }, {200.f, 50.f});
		Text thread(font, "No. of threads", 30, { 25.f,330.f }, sf::Color::Black, sf::Text::Regular);
		Button thread_1("1",font,{ 20.f, 380.f }, {200.f,50.f});
		Button thread_2("2",font,{ 20.f, 440.f }, {200.f,50.f});
		Button thread_4("4",font,{ 20.f, 500.f }, {200.f,50.f});
		Button thread_8("8",font,{ 20.f, 560.f }, {200.f,50.f});
		Button startbutton("START",font,{ 20.f, 690.f }, {200.f,50.f});	
		Message message(font, "Choose DLL, no. of threads and an image.", 20, width, height);
		Text before_image(font, "Input image:", 40, sf::Vector2f( width / 3.f-50,height / 2.f -400.f  ), sf::Color::Black, sf::Text::Bold);
		Text after_image(font, "Output image:", 40, sf::Vector2f(width / 3.f*2.f , height / 2.f - 400.f), sf::Color::Black, sf::Text::Bold);
		Button select_image("SELECT IMAGE", font,{width/3.f-25,height/2.f-100}, { 200.f,50.f });
		Button select_other_image("SELECT OTHER IMAGE", font, { width / 3.f-50,height - 200.f }, { 250.f,50.f });
		Button save_button("SAVE IMAGE", font, { width / 3.f * 2 + 5,height - 200.f }, { 250.f,50.f });
		sf::Clock clock;

		sf::Texture image_be;
		sf::Image Image;
		std::unique_ptr<sf::Sprite> be_sprite;
		std::unique_ptr<sf::Sprite> af_sprite;
		
		bool imageloaded = false;
		while (window.isOpen())
		{
			while (const std::optional event = window.pollEvent())
			{
				if (event->is<sf::Event::Closed>())
					window.close();
				if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
					if (mouse->button == sf::Mouse::Button::Left) {
						sf::Vector2f mousePosF(static_cast<float>(mouse->position.x),
							static_cast<float>(mouse->position.y));
						if (libcpp.isClicked(mousePosF)) {
							libcpp.active = true;
							libasm.active = false;
						}
						else if (libasm.isClicked(mousePosF)) {
							libcpp.active = false;
							libasm.active = true;
						}
						else if (thread_1.isClicked(mousePosF)) {
							no_of_threads = 1;
							thread_1.active = true;
							thread_2.active = false;
							thread_4.active = false;
							thread_8.active = false;
						}
						else if (thread_2.isClicked(mousePosF)) {
							no_of_threads = 2;
							thread_1.active = false;
							thread_2.active = true;
							thread_4.active = false;
							thread_8.active = false;
						}
						else if (thread_4.isClicked(mousePosF)) {
							no_of_threads = 4;
							thread_1.active = false;
							thread_2.active = false;
							thread_4.active = true;
							thread_8.active = false;
						}
						else if (thread_8.isClicked(mousePosF)) {
							no_of_threads = 8;
							thread_1.active = false;
							thread_2.active = false;
							thread_4.active = false;
							thread_8.active = true;
						}
						else if (startbutton.isClicked(mousePosF))
						{
							if ((no_of_threads == 0) && ((libcpp.active == false) && (libasm.active == false) && imageloaded == false))
							{
								message.setString("Choose DLL, no. of threads and an image.", width, height);
							}
							else if ((no_of_threads == 0) && ((libcpp.active == false) && (libasm.active == false)) && (imageloaded == true))
							{
								message.setString("Choose DLL and no. of threads.", width, height);
							}
							else if ((no_of_threads == 0) && ((libcpp.active == true) || (libasm.active == true)) && (imageloaded == true))
							{
								message.setString("Choose no. of threads.", width, height);

							}
							else if ((no_of_threads > 0) && ((libcpp.active == false) && (libasm.active == false)) && (imageloaded == true))
							{
								message.setString("Choose DLL.", width, height);

							}
							else if ((no_of_threads == 0) && (libcpp.active == true) || (libasm.active == true) && (imageloaded == false))
							{
								message.setString("Choose no. of threads and an image.", width, height);
							}
							else if ((no_of_threads > 0) && ((libcpp.active == false) && (libasm.active == false)) && (imageloaded == false))
							{
								message.setString("Choose DLL and an image.", width, height);
							}
							else if ((no_of_threads > 0) && ((libcpp.active == true) || (libasm.active == true)) && imageloaded == false)
							{
								message.setString("Select an image.", width, height);
							}
							else
							{
								auto start = std::chrono::high_resolution_clock::now();
								message.setString("Simulating...", width, height);
								{
									std::this_thread::sleep_for(std::chrono::seconds(1));
									message.setString("Processing Grayscale...", width, height);

									std::thread processingThread([&]() {

										sf::Vector2u size = Image.getSize();
										sf::Image grayImage(sf::Vector2u(size.x, size.y));
										sf::Image gaussImage(sf::Vector2u(size.x, size.y));
										sf::Image gradientImage(sf::Vector2u(size.x, size.y));
										sf::Image suspensionImage(sf::Vector2u(size.x, size.y));
										sf::Image ThresholdImage(sf::Vector2u(size.x, size.y));
										sf::Image FinalImage(sf::Vector2u(size.x, size.y));
										std::vector<float> direction(size.x * size.y);
										std::vector<weakPixel> weakPixels;
										float maxVal = 0;
										// Etap 1 – grayscale
										processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
											convertToGray(Image, grayImage,size, startY, endY);
											});
										{
											std::lock_guard<std::mutex> lock(imageMutex);
											processedImage = grayImage;
										}
										currentStage = ProcessingStage::Grayscale;
										stageUpdated = true;
										// Etap 2 – Gaussian blur
										processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
											gaussFilter(grayImage, gaussImage,size, startY, endY);
											});
										{
											std::lock_guard<std::mutex> lock(imageMutex);
											processedImage = gaussImage;
										}
										currentStage = ProcessingStage::Gaussian;
										stageUpdated = true;
										//Etap 3 - Obliczanie gradientu
										processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
											gradient(gaussImage, gradientImage,direction,size, startY, endY);
											});
										{
											std::lock_guard<std::mutex> lock(imageMutex);
											processedImage = gradientImage;
										}
										currentStage = ProcessingStage::Gradient;
										stageUpdated = true;
										//Etap 4 - Supresja lokalnych nie maksimów
										processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
											nonMaximumSuspension(gradientImage, suspensionImage,direction,size,maxVal, startY, endY);
											});
										{
											std::lock_guard<std::mutex> lock(imageMutex);
											processedImage = suspensionImage;
										}
										currentStage = ProcessingStage::NonMaximalSuspention;
										stageUpdated = true;
										//Etap 5 - Podwójne progowanie
										float strongPixel = 0.2*maxVal;
										float wheakPixel = 0.4 * strongPixel;
										processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
											doubleThresholding(suspensionImage, ThresholdImage,weakPixels,strongPixel,wheakPixel,size, startY, endY);
											});
										{
											std::lock_guard<std::mutex> lock(imageMutex);
											processedImage = ThresholdImage;
										}
										currentStage = ProcessingStage::DoubleThreshold;
										stageUpdated = true;
										//Etap 6 - Histereza
										processInThreads(no_of_threads, size.y, [&](unsigned int startY, unsigned int endY) {
											histerezis(ThresholdImage, FinalImage, size,weakPixels,strongPixel, startY, endY);
											});
										{
											std::lock_guard<std::mutex> lock(imageMutex);
											processedImage = ThresholdImage;
										}	
										currentStage = ProcessingStage::Hysteresis;
										auto end = std::chrono::high_resolution_clock::now();
										stageUpdated = true;
										auto cpp_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
										double processingTimeMs = cpp_time/1000.0;
										});
									processingThread.detach();
								}
							}
						}
						else if (select_image.isClicked(mousePosF))
						{
							std::string imagePath = openFileDialog();
							if (!imagePath.empty())
								if (!Image.loadFromFile(imagePath)) {
									std::cerr << "Nie uda³o siê za³adowaæ obrazka!\n";
								}
								else if (!image_be.loadFromFile(imagePath))
								{
									std::cerr << "Nie uda³o siê za³adowaæ obrazka!\n";
								}
								else {
									imageloaded = true;
									be_sprite = std::make_unique<sf::Sprite>(image_be);
									sf::Vector2f scale;
									scale = rescall_Image(image_be, width / 2.6, height / 2.f);
									be_sprite->setScale(scale);
									sf::Vector2u pos = image_be.getSize();
									if (pos.x > pos.y)
									{
										be_sprite->setPosition(sf::Vector2f(width / 3.f - 350, height / 2.f - 300));
									}
									else
									{
										be_sprite->setPosition(sf::Vector2f(width / 3.f - 200, height / 2.f - 300));
									}
								}
						}
						else if (select_other_image.isClicked(mousePosF))
						{
							std::string imagePath = openFileDialog();
							if (!imagePath.empty())
								if (!Image.loadFromFile(imagePath)) {
									std::cerr << "Nie uda³o siê za³adowaæ obrazka!\n";
								}
								else if (!image_be.loadFromFile(imagePath))
								{
									std::cerr << "Nie uda³o siê za³adowaæ obrazka!\n";
								}
								else {
									imageloaded = true;
									be_sprite = std::make_unique<sf::Sprite>(image_be);
									sf::Vector2f scale = rescall_Image(image_be, width / 2.6, height / 2.f);
									be_sprite->setScale(scale);
									sf::Vector2u pos = image_be.getSize();
									if (pos.x > pos.y)
									{
										be_sprite->setPosition(sf::Vector2f(width / 3.f - 350, height / 2.f - 300));
									}
									else
									{
										be_sprite->setPosition(sf::Vector2f(width / 3.f - 200, height / 2.f - 300));
									}
								}
						}
						else if (save_button.isClicked(mousePosF))
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


						window.clear();
						window.draw(background);
						rect1.draw(window);
						rect2.draw(window);
						title.draw(window);
						lib.draw(window);
						libcpp.draw(window);
						libasm.draw(window);
						thread.draw(window);
						thread_1.draw(window);
						thread_2.draw(window);
						thread_4.draw(window);
						thread_8.draw(window);
						startbutton.draw(window);
						before_image.draw(window);
						after_image.draw(window);
						if (!imageloaded)
							select_image.draw(window);
						message.draw(window);
						if (imageloaded)
						{
							window.draw(*be_sprite);
							select_other_image.draw(window);
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

									sf::Vector2u pos = image_be.getSize();
									if (pos.x > pos.y)
										sprite.setPosition(sf::Vector2f(width / 3.f * 2.f - 200, height / 2.f - 300));
									else
										sprite.setPosition(sf::Vector2f(width / 3.f * 2.f, height / 2.f - 300));
								}
							}
							stageUpdated = false;
						}
						if (currentStage != ProcessingStage::None) {
							window.draw(sprite);
							save_button.draw(window);
							std::string stageName;
							switch (currentStage) {
							case ProcessingStage::Grayscale: stageName = "Gaussian Blur..."; break;
							case ProcessingStage::Gaussian: stageName = "Gradient..."; break;
							case ProcessingStage::Gradient: stageName = "NonMaximalSuspention..."; break;
							case ProcessingStage::NonMaximalSuspention: stageName = "DoubleThreshold..."; break;
							case ProcessingStage::DoubleThreshold: stageName = "Hysterezis..."; break;
							case ProcessingStage::Hysteresis: stageName = "Done."; break;						default: break;
							}

							message.setString("Procesing stage: " + stageName, width, height);

								
						}
						if (currentStage == ProcessingStage::Hysteresis) {
							Text time(font,
								"Work time: " + std::to_string(processingTimeMs.load()) + " s",
								20,
								sf::Vector2f(width / 2.f - 100, height - 200.f),
								sf::Color::Black,
								sf::Text::Bold);
							time.draw(window);
						}
						window.display();
					}
				
			
		

};