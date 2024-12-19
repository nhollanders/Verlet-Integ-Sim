#pragma once
// std includes
#include <vector>
#include <iostream>

// SFML includes
#include <SFML/Graphics.hpp>

// custom includes
#include "VerletObject.cpp"
#include "util/math.h"

struct PhysSolver
{
	sf::Vector2f gravity = {0.0f, 1000.0f};
	sf::CircleShape backgroundCircle;
	std::vector<VerletObject> verletObjects;

	// object data
	const int sub_steps = 8; // phys substeps
	const float obj_radius = 2.f; // radius of the balls
	const float collider_radius = 300.f; // radius of the collider

	PhysSolver()
	{
		sf::CircleShape backgroundCirclet{ collider_radius };
		backgroundCirclet.setOrigin(collider_radius, collider_radius);
		backgroundCirclet.setFillColor(sf::Color::White);
		backgroundCirclet.setRadius(collider_radius);
		backgroundCirclet.setPosition(400.f, 300.f);
		backgroundCirclet.setPointCount(128);

		this->backgroundCircle = backgroundCirclet;
	}

	void addVerletObject(sf::Vector2f pos)
	{
		VerletObject newObject = VerletObject(pos, obj_radius, verletObjects.size() + 1);
		verletObjects.push_back(newObject);
	}

	void update(float dt)
	{
		const float sub_dt = dt / static_cast<float>(sub_steps);
		for (size_t i(sub_steps); i--;)
		{
			applyGravity();
			applyConstraint();
			findCollisions();
			updatePositions(sub_dt);
		}


		//std::cout << "Ball Count: " << this->verletObjects.size() << std::endl;
	}

	void updatePositions(float dt)
	{
		for (VerletObject& obj : verletObjects)
		{
			obj.updatePosition(dt);
		}
	}

	void applyGravity()
	{
		for (VerletObject& obj : verletObjects)
		{
			obj.accelerate(this->gravity);
		}
	}

	void applyConstraint()
	{
		const sf::Vector2f position = (this->backgroundCircle.getPosition() - sf::Vector2f(obj_radius, obj_radius));
		const float radius = this->backgroundCircle.getRadius();

		for (auto& obj : this->verletObjects) {
			const float objRad = obj.shape.getRadius();
			const sf::Vector2f v = position - obj.curPos;
			const float        dist = sqrt(v.x * v.x + v.y * v.y);
			if (dist > (radius - objRad)) {
				const sf::Vector2f n = v / dist;
				obj.curPos = position - n * (radius - objRad);
			}
		}
	}

	// determines if two objects are colliding or not
	bool collides(VerletObject& obj1, VerletObject& obj2)
	{
		float minDist = obj1.shape.getRadius() + obj2.shape.getRadius();
		return EuclideanDist2DSqrt(obj1.curPos, obj2.curPos) < (minDist * minDist);
	}

	// TODO: this needs to be more stable because rn is quite unstable compared to the last version
	void solveCollision(VerletObject& obj1, VerletObject& obj2)
	{
		constexpr float eps = 0.0001f;
		const float dist2 = EuclideanDist2DSqrt(obj1.curPos, obj2.curPos);
		const float minDist = obj1.shape.getRadius() + obj2.shape.getRadius();
		const float minDist2 = minDist * minDist; // Squared minimum distance

		if (dist2 < minDist2 && dist2 > eps) {
			const float dist = sqrt(dist2);
			const float overlap = 0.5f * (minDist - dist);

			// Normalize delta and displace the objects to resolve the collision
			sf::Vector2f delta = obj1.curPos - obj2.curPos;
			sf::Vector2f displacement = overlap * (delta / dist);
			obj1.curPos += displacement;
			obj2.curPos -= displacement;
		}
	}
	
	void findCollisions()
	{
		for (auto& obj_1 : verletObjects)
		{
			for (auto& obj_2 : verletObjects)
			{
				if (obj_1 != obj_2)
				{
					if (collides(obj_1, obj_2))
					{
						solveCollision(obj_1, obj_2);
					}
				}
			}
		}
	}

	void render(sf::RenderWindow* window)
	{
		window->draw(backgroundCircle);

		for (VerletObject& obj : verletObjects)
		{
			obj.render(window);
		}
	}
};