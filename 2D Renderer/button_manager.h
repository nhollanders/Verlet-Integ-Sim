#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <utility>
#include "button.cpp"

class button_manager
{
private:
	std::vector<SquareButton> managedButtons; // all buttons to manage the updates of
	std::vector<std::pair<SquareButton*, sf::Color>> pressedButtons; // buttons pressed that need their colors reset
	bool buttonPressed = false;

public:
	SquareButton* AddButton(std::string title, sf::Vector2f position, sf::Vector2f size, std::function<void(SquareButton* button)> function, sf::Font& font)
	{
		SquareButton newButton = SquareButton(title, position, size, function, font);
		managedButtons.push_back(newButton);
		return &newButton;
	}

	void update(sf::RenderWindow& window)
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !buttonPressed) // if mouse gets clicked
		{
			buttonPressed = true;
			sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window)); // gets mouse position reletive to the window
			for (SquareButton& obj : managedButtons)
			{
				if (obj.buttShape.getGlobalBounds().contains(mousePos)) // if the mouse clicked while inside the box
				{
					pressedButtons.push_back(std::make_pair(&obj, obj.buttShape.getFillColor()));
					obj.buttShape.setFillColor(obj.buttShape.getFillColor() + sf::Color(50, 50, 50, 0));
					obj.press();
				}
			}
		}

		if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) // not clicked
		{
			buttonPressed = false;

			for (std::pair<SquareButton*, sf::Color> obj : pressedButtons)
			{
				obj.first->buttShape.setFillColor(obj.second);
			}
		}
	}

	void render(sf::RenderWindow& window)
	{
		for (SquareButton& obj : managedButtons)
		{
			obj.render(window);
		}
	}
};
