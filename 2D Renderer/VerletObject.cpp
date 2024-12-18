#pragma once

// SFML includes
#include <SFML/Graphics.hpp>

#include <iostream>

using namespace std;

struct VerletObject
{
	sf::Vector2f curPos;
	sf::Vector2f lastPos;
	sf::Vector2f acceleration;

	sf::CircleShape shape;

	VerletObject(sf::Vector2f startPos, float rad)
	{

		shape.setFillColor(sf::Color(50,50,50,255));
		shape.setRadius(rad);

		shape.setPosition(startPos);
		curPos = startPos;
		lastPos = startPos;
		acceleration = sf::Vector2f(0.f, 0.f);
	}

	void accelerate(sf::Vector2f acc)
	{
		this->acceleration += acc;
	}
	
	void updatePosition(float dt)
	{
		const sf::Vector2f velocity = curPos - lastPos;

		lastPos = curPos;

		curPos = curPos + velocity + acceleration * (dt * dt);

		acceleration = {};

		shape.setPosition(curPos);
	}

	void render(sf::RenderWindow* window)
	{
		window->draw(shape);
	}
};