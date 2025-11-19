#include "GUI.h"

	Message::Message(const sf::Font& font, const std::string& str, unsigned int charSize, unsigned int windowWidth, unsigned int windowHeight)
		:text(font, str, charSize)
	{
		text.setFillColor(sf::Color::Black);
		text.setStyle(sf::Text::Bold);
		auto bounds = text.getLocalBounds();
		text.setOrigin(sf::Vector2f(bounds.size.x / 2.f, bounds.size.y / 2.f));
		text.setPosition(sf::Vector2f((windowWidth) / 2.f, (windowHeight)-80));
	}

	void Message::draw(sf::RenderWindow& window) {
		window.draw(text);
	}

	void Message::setString(const std::string& str, unsigned int windowWidth, unsigned int windowHeight) {
		text.setString(str);
		auto bounds = text.getLocalBounds();
		text.setOrigin(sf::Vector2f(bounds.size.x / 2.f, bounds.size.y / 2.f));
		text.setPosition(sf::Vector2f((windowWidth) / 2.f, (windowHeight)-80));
	}

	Button::Button(const std::string& label, sf::Font& font, sf::Vector2f position, sf::Vector2f size, unsigned int charSize)
		: text(font, label, charSize)
	{
		shape.setSize(size);
		shape.setPosition(position);
		shape.setFillColor(sf::Color(100, 100, 100));
		text.setPosition(sf::Vector2f(position.x + 10.f, position.y + 10.f));
	}
	void Button::draw(sf::RenderWindow& window) {
		shape.setFillColor(active ? sf::Color::Green : sf::Color(100, 100, 100));
		window.draw(shape);
		window.draw(text);
	}

	bool Button::isClicked(sf::Vector2f mousePos) const {
		return shape.getGlobalBounds().contains(sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)));
	}
	Rect::Rect(float width, float height, sf::Vector2f position, sf::Color fillColor, sf::Color color, int thickless)
	{
		shape.setSize(sf::Vector2f(width, height));
		shape.setPosition(position);
		shape.setFillColor(fillColor);
		shape.setOutlineColor(color);
		shape.setOutlineThickness(thickless);
	}
	void Rect::draw(sf::RenderWindow& window)
	{
		window.draw(shape);
	}
	Text::Text(sf::Font& font, const std::string str, unsigned int charSize, sf::Vector2f position, sf::Color fillColor, sf::Text::Style style)
		:text(font, str, charSize)
	{
		text.setPosition(position);
		text.setFillColor(fillColor);
		text.setStyle(style);
	}
	void Text::draw(sf::RenderWindow& window) {
		window.draw(text);
	}
	sf::Vector2f rescall_Image(sf::Texture& tex, float maxX, float maxY)
	{
		float scaleX, scaleY;
		sf::Vector2u size = tex.getSize();
		if (size.x == 0 || size.y == 0)
			return { 1.f, 1.f };
		if (size.x >= size.y)
		{
			scaleX = maxX / size.x;
			scaleY = scaleX;
		}
		else
		{
			scaleY = maxY / size.y;
			scaleX = scaleY;
		}
		return { scaleX, scaleY };
	}

