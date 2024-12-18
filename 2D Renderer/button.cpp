#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

struct SquareButton
{
	sf::RectangleShape buttShape;
	std::string butText;
	std::function<void()> buttonPressed; // Add this line to store the callback
	sf::Text buttonText;

	void updateRendering(sf::Font font)
	{
		buttonText.setFont(font);
		buttonText.setCharacterSize(24);
		buttonText.setFillColor(sf::Color::White);
		buttonText.setString(this->butText);
	}

	SquareButton(std::string title, sf::Vector2f position, sf::Font font)
	{
		butText = title;
		buttShape.setPosition(position);
		buttShape.setFillColor(sf::Color::Black);
		buttShape.setOutlineColor(sf::Color::White);
		buttShape.setSize(sf::Vector2f(50.f, 50.f));

		updateRendering(font);

		buttonPressed = nullptr; // Initialize the callback to nullptr
	}

	void render(sf::RenderWindow& window)
	{
		window.draw(this->buttShape);
	}

	void press() // Add this method to call the callback
	{
		if (buttonPressed)
		{
			buttonPressed();
		}
	}
};