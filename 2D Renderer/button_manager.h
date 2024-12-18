#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include "button.cpp"

class button_manager
{
private:
	std::vector<SquareButton> managedButtons; // all buttons to manage the updates of
	bool buttonPressed = false;

public:
	SquareButton* AddButton(std::string title, sf::Vector2f position, sf::Font font)
	{
		SquareButton newButton = SquareButton(title, position, font);
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
					obj.press();
				}
			}
		}

		if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) // not clicked
		{
			buttonPressed = false;
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
