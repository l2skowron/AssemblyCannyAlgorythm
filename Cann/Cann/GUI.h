#pragma once
#include<SFML/Graphics.hpp>
#include<string>

struct Message {
	sf::Text text;
	Message(const sf::Font& font, const std::string& str, unsigned int charSize, unsigned int windowWidth, unsigned int windowHeight);
	void draw(sf::RenderWindow& window);
	void setString(const std::string& str, unsigned int windowWidth, unsigned int windowHeight);	
};
struct Button {
	sf::RectangleShape shape;
	sf::Text text;
	bool active = false;


	Button(const std::string& label, sf::Font& font, sf::Vector2f position, sf::Vector2f size, unsigned int charSize=20);
	void draw(sf::RenderWindow& window);
	bool isClicked(sf::Vector2f mousePos) const;
};
struct Rect {
	sf::RectangleShape shape;
	Rect(float width, float height, sf::Vector2f position, sf::Color fillColor, sf::Color color, int thickless);
	void draw(sf::RenderWindow& window);
};
struct Text {
	sf::Text text;

	Text(sf::Font& font, const std::string str, unsigned int charSize, sf::Vector2f position, sf::Color fillColor, sf::Text::Style style);
	void draw(sf::RenderWindow& window);
};
sf::Vector2f rescall_Image(sf::Texture& tex, float maxX, float maxY);
