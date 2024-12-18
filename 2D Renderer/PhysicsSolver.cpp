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
	const float obj_radius = 5.f; // radius of the balls
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
		VerletObject newObject = VerletObject(pos, obj_radius);
		verletObjects.push_back(newObject);
	}

	void update(float dt)
	{
		const float sub_dt = dt / static_cast<float>(sub_steps);
		for (size_t i(sub_steps); i--;)
		{
			applyGravity();
			applyConstraint();
			solveCollisions();
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
			const sf::Vector2f v = position - obj.curPos;
			const float        dist = sqrt(v.x * v.x + v.y * v.y);
			if (dist > (radius - obj_radius)) {
				const sf::Vector2f n = v / dist;
				obj.curPos = position - n * (radius - obj_radius);
			}
		}
	}

	void solveCollisions()
	{
		const float    response_coef = 0.75f;
		const size_t objects_count = verletObjects.size();
		// Iterate on all objects
		for (size_t i{ 0 }; i < objects_count; ++i) {
			VerletObject& object_1 = verletObjects[i];
			// Iterate on object involved in new collision pairs
			for (size_t k{ i + 1 }; k < objects_count; ++k) {
				VerletObject& object_2 = verletObjects[k];
				const sf::Vector2f v = object_1.curPos - object_2.curPos;
				const float        dist2 = v.x * v.x + v.y * v.y;
				const float        min_dist = obj_radius + obj_radius;
				// Check overlapping
				if (dist2 < min_dist * min_dist) {
					const float        dist = sqrt(dist2);
					const sf::Vector2f n = v / dist;
					const float mass_ratio_1 = obj_radius / (obj_radius + obj_radius);
					const float mass_ratio_2 = obj_radius / (obj_radius + obj_radius);
					const float delta = 0.5f * response_coef * (dist - min_dist);
					// Update positions
					object_1.curPos -= n * (mass_ratio_2 * delta);
					object_2.curPos += n * (mass_ratio_1 * delta);
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