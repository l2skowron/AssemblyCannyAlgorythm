#include "GUI.h"


std::atomic<ProcessingStage> currentStage = ProcessingStage::None;
std::atomic<bool> stageUpdated = false;
std::mutex imageMutex;
sf::Image processedImage;

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
	};
	void imageToVector(const sf::Image& image, std::vector<uint8_t>& vec, Size size)
	{
		vec.resize(size.x * size.y);
		for (unsigned int y = 0; y < size.y; ++y) {
			for (unsigned int x = 0; x < size.x; ++x)
			{
				sf::Color c = image.getPixel(sf::Vector2u(x, y));
				vec[y * size.x + x] = c.r;
			}
		}
	};
	void vectorToImage(const std::vector<uint8_t>& vec, sf::Image& image, Size size, unsigned int startY, unsigned int endY)
	{
		for (unsigned int y = startY; y < endY; ++y) {
			for (unsigned int x = 0; x < size.x; ++x)
			{
				uint8_t val = vec[y * size.x + x];
				image.setPixel(sf::Vector2u(x, y), sf::Color(val, val, val));
			}
		}
	};
	bool isBMP(const std::string& path) {
		std::filesystem::path p(path);
		std::string ext = p.extension().string();

		return ext == ".bmp";
	};

	GuiContext::GuiContext()
		: window(sf::VideoMode::getDesktopMode(),
			"Canny detection algorithm",
			sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize)
	{

		// Wczytywanie fontu
		auto fontPath = std::filesystem::absolute("ARLRDBD.TTF");
		if (!font.openFromFile(fontPath.string()))
			throw std::runtime_error("Couldn't load font from: " + fontPath.string());

		// T³o
		backgroundTexture.loadFromFile("background.png");
		background.emplace(backgroundTexture);
		background->setScale(sf::Vector2f(1, 1));
		background->setPosition({ 0,0 });

		if(!icon.loadFromFile("drzewo.bmp"))
			throw std::runtime_error("Couldn't load icon");
		window.setIcon(icon);
		sf::View view(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(1920, 1080)));
		window.setView(view);
		// Inicjalizacja UI:
		unsigned int width = window.getSize().x;
		unsigned int height = window.getSize().y;
		title.emplace(font, "Application of the Canny edge detection algorithm.",
			50, sf::Vector2f({ window.getSize().x / 6.f, 30.f }), sf::Color::Black, sf::Text::Bold);
		rect1.emplace(width - 300, height - 220, sf::Vector2f{ 250,110 }, sf::Color::Transparent, sf::Color::Black, 2);
		rect2.emplace(246, height - 220, sf::Vector2f{ 3,110 }, sf::Color::Transparent, sf::Color::Black, 2);
		lib.emplace(font, "Choose DDL", 30, sf::Vector2f{ 25.f, 150.f }, sf::Color::Black, sf::Text::Regular);
		message.emplace(font, "", 20, width, height);
		timeMessage.emplace(font, "", 20,width/ 3.f +25, height -200.f);
		libcpp.emplace("C++", font, sf::Vector2f{ 20.f, 200.f }, sf::Vector2f{ 200.f, 50.f });
		libasm.emplace("Asm", font, sf::Vector2f{ 20.f, 260.f }, sf::Vector2f{ 200.f, 50.f });
		startbutton.emplace("START", font, sf::Vector2f{ 20.f, 690.f }, sf::Vector2f{ 200.f,50.f });
		before_image.emplace(font, "Input image: ", 40, sf::Vector2f(width / 3.f - 50, height / 2.f - 400.f), sf::Color::Black, sf::Text::Bold);
		after_image.emplace(font, "Output image:", 40, sf::Vector2f(width / 3.f * 2.f, height / 2.f - 400.f), sf::Color::Black, sf::Text::Bold);
		select_image.emplace("SELECT IMAGE", font, sf::Vector2f{ width / 3.f - 25,height / 2.f - 100 }, sf::Vector2f{ 200.f,50.f });
		select_other_image.emplace("SELECT OTHER IMAGE", font, sf::Vector2f{ width / 3.f - 50,height - 200.f }, sf::Vector2f{ 250.f,50.f });
		save_button.emplace("SAVE IMAGE", font, sf::Vector2f{ width / 3.f * 2 + 5,height - 200.f }, sf::Vector2f{ 250.f,50.f });
		thread.emplace(font, "No. of threads", 30, sf::Vector2f{ 25.f, 330.f }, sf::Color::Black, sf::Text::Regular);
		thread_1.emplace("1", font, sf::Vector2f{ 20.f, 380.f }, sf::Vector2f{ 200.f,50.f });
		thread_2.emplace("2", font, sf::Vector2f{ 20.f, 440.f }, sf::Vector2f{ 200.f,50.f });
		thread_4.emplace("4", font, sf::Vector2f{ 20.f, 500.f }, sf::Vector2f{ 200.f,50.f });
		thread_8.emplace("8", font, sf::Vector2f{ 20.f, 560.f }, sf::Vector2f{ 200.f,50.f });
		thread_16.emplace("16", font, sf::Vector2f{ 20.f, 620.f }, sf::Vector2f{ 200.f,50.f });
		thread_32.emplace("32", font, sf::Vector2f{ 20.f, 680.f }, sf::Vector2f{ 200.f,50.f });
		thread_64.emplace("64", font, sf::Vector2f{ 20.f, 740.f }, sf::Vector2f{ 200.f,50.f });
		startbutton.emplace("START", font, sf::Vector2f{ 20.f, 890.f }, sf::Vector2f{ 200.f,50.f });
		presentation.emplace("PRESENTATION", font, sf::Vector2f{ 20.f, 800.f}, sf::Vector2f{ 200.f,50.f });
	}

