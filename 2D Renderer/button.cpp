#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <iostream>
#include <sstream>

struct SquareButton
{
	sf::RectangleShape buttShape;
	std::string butText;
	std::function<void()> buttonPressed; // Add this line to store the callback
	sf::Text buttonText;

    void setPressFunction(std::function<void()> function)
    {
        this->buttonPressed = function;
    }

	void updateRendering(sf::Font font)
	{
		buttonText.setFont(font);
		buttonText.setCharacterSize(12);
		buttonText.setFillColor(sf::Color::White);

		std::stringstream ss;
		ss << this->butText;

		buttonText.setString(ss.str());
		buttonText.setPosition(this->buttShape.getPosition());
	}

	SquareButton(std::string title, sf::Vector2f position, sf::Font font, std::function<void()> function)
	{
		this->setPressFunction(function);

		butText = title;
		buttShape.setPosition(position);
		buttShape.setFillColor(sf::Color(100, 100, 100, 255));
		buttShape.setOutlineColor(sf::Color::White);
		buttShape.setSize(sf::Vector2f(50.f, 50.f));
		buttShape.setOutlineThickness(2.f);

		updateRendering(font);

		this->buttonPressed = nullptr; // Initialize the callback to nullptr
	}

	void render(sf::RenderWindow& window)
	{
		window.draw(this->buttShape);
		//window.draw(this->buttonText);
	}

	void press() // Add this method to call the callback
	{
		if (this->buttonPressed)
		{
			this->buttonPressed();
		}
	}
};