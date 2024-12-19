#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <iostream>
#include <sstream>

struct SquareButton
{
	sf::RectangleShape buttShape;
	std::string butText;
	std::function<void(SquareButton* button)> buttonPressed; // Add this line to store the callback
	sf::Text buttonText;

    void setPressFunction(std::function<void(SquareButton* button)> function)
    {
        this->buttonPressed = function;
    }

	void updateRendering(sf::Font& font)
	{
		buttonText.setFont(font);
		buttonText.setCharacterSize(12);
		buttonText.setFillColor(sf::Color::White);

		std::stringstream ss;
		ss << this->butText;

		buttonText.setString(ss.str());
		auto shapePos = this->buttShape.getPosition();
		// getting width and height of text is broken in SFML since 2012 apparently so guess I will not be adding this for now due to the needed work.
		//auto textBoundsSize = buttonText.getLocalBounds();
		//auto centerOrigin = sf::Vector2f(textBoundsSize.width / 2.f, textBoundsSize.height / 2.f);

		/*
		std::cout << "Button made: \nBoxPos: " << shapePos.x << ", " << shapePos.y
			<< "\nNew Origin: " << centerOrigin.x << ", " << centerOrigin.y << "\n";
		*/

		shapePos.x += 2.f;

		//buttonText.setOrigin(centerOrigin);
		buttonText.setPosition(shapePos);
	}

	SquareButton(std::string title, sf::Vector2f position, sf::Vector2f size, std::function<void(SquareButton* button)> function, sf::Font& font)
	{
		butText = title;
		buttShape.setPosition(position);
		buttShape.setSize(size);

		buttShape.setOutlineThickness(2.f);

		buttShape.setFillColor(sf::Color(100, 100, 100, 255));
		buttShape.setOutlineColor(sf::Color::White);

		updateRendering(font); // update text stuff

		this->buttonPressed = nullptr; // Initialize the callback to nullptr

		this->setPressFunction(function);
	}

	void render(sf::RenderWindow& window)
	{
		window.draw(this->buttShape);
		window.draw(this->buttonText);
	}

	void press() // Add this method to call the callback
	{
		if (this->buttonPressed)
		{
			this->buttonPressed(this);
		}
	}
};