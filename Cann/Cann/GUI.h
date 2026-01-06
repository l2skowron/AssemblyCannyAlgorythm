#pragma once
#include<SFML/Graphics.hpp>
#include<string>
#include<atomic>
#include<mutex>
#include "BMP.h"

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

struct Size {
	uint32_t x;
	uint32_t y;
};
extern std::atomic<ProcessingStage> currentStage;
extern std::atomic<bool> stageUpdated;
extern std::mutex imageMutex;
extern sf::Image processedImage;
class Message{
public:
	
	Message(const sf::Font& font, const std::string& str, unsigned int charSize, unsigned int windowWidth, unsigned int windowHeight);
	void draw(sf::RenderWindow& window);
	void setString(const std::string& str, unsigned int windowWidth, unsigned int windowHeight);
private:
	sf::Text text;
};
class Button {
public:

	Button(const std::string& label, sf::Font& font, sf::Vector2f position, sf::Vector2f size, unsigned int charSize=20);
	void draw(sf::RenderWindow& window);
	bool isClicked(sf::Vector2f mousePos) const;
	void setActive(bool value) { active = value; }
	bool getActive() const { return active; }
private:
	sf::RectangleShape shape;
	sf::Text text;
	bool active = false;
};
class Rect {
public:
	Rect(float width, float height, sf::Vector2f position, sf::Color fillColor, sf::Color color, int thickless);
	void draw(sf::RenderWindow& window);
private:
	sf::RectangleShape shape;
};
class Text {
public:

	Text(sf::Font& font, const std::string str, unsigned int charSize, sf::Vector2f position, sf::Color fillColor, sf::Text::Style style);
	void draw(sf::RenderWindow& window);
private:
	sf::Text text;
};
sf::Vector2f rescall_Image(sf::Texture& tex, float maxX, float maxY);
void imageToVector(const sf::Image& image, std::vector<uint8_t>& vec, Size size);
void vectorToImage(const std::vector<uint8_t>& vec, sf::Image& image, Size size, unsigned int startY, unsigned int endY);
bool isBMP(const std::string& path);

class GuiContext {
public:
    sf::RenderWindow window;
    sf::Texture backgroundTexture;
	std::optional< sf::Sprite> background;
	std::optional<Message> message;
	std::optional<Message> timeMessage;
	sf::Image icon;
    sf::Font font;

    // Twoje UI
	std::optional<Rect> rect1;
	std::optional<Rect> rect2;
    std::optional<Text> title;
    std::optional<Text> lib;
    std::optional<Button> libcpp;
    std::optional<Button> libasm;
    std::optional<Text> thread;
	std::optional < Button> thread_1, thread_2, thread_4, thread_8, thread_16, thread_32, thread_64;
	std::optional < Button> startbutton;
	std::optional < Text> before_image;
	std::optional < Text> after_image;
	std::optional < Button> select_image;
	std::optional < Button> select_other_image;
	std::optional < Button> save_button;
	std::optional <Button> presentation;

    // Obrazy
    sf::Texture image_be;
    sf::Image Image;
    BMP bmpImage;
    std::optional<sf::Sprite> be_sprite;
    std::optional<sf::Sprite> af_sprite;

    bool imageloaded = false;

    std::atomic<long long> processingTimeMs{ 0 };

    GuiContext();
};
