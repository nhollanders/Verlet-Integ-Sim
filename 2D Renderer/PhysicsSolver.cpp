#pragma once
// std includes
#include <vector>
#include <iostream>

// SFML includes
#include <SFML/Graphics.hpp>

// custom includes
#include "VerletGrid.cpp"
#include "VerletObject.cpp"
#include "util/math.h"


struct PhysSolver
{
	// physics data
	sf::Vector2f gravity = {0.0f, 1000.0f}; // x and y gravity
	sf::CircleShape backgroundCircle; // this circle is basically the white circle in the back, reffered to for data about collisions.

	// data collections
	std::vector<VerletObject*> verletObjList; // list of all the content
	VerletGrid verletScreenGrid; // verlet grid

	// phys objects data
	const float sub_steps = 4.f; // phys substeps
	const float obj_radius = 4.f; // radius of the balls
	const float collider_radius = 300.f; // radius of the collider
	const sf::Vector2f collider_pos = sf::Vector2f(400.f, 300.f);
	
	PhysSolver() // constructor
	{

		// constructs the circle that determines physics pos and data.
		sf::CircleShape backgroundCirclet{ collider_radius };
		backgroundCirclet.setOrigin(collider_radius, collider_radius);
		backgroundCirclet.setFillColor(sf::Color::White);
		backgroundCirclet.setRadius(collider_radius);
		backgroundCirclet.setPosition(collider_pos);
		backgroundCirclet.setPointCount(128);

		this->backgroundCircle = backgroundCirclet;
	}

    void addVerletObject(sf::Vector2f pos) // adds a ball to the simulation at a given position
    {
		VerletObject* newObj = new VerletObject(pos, obj_radius, verletObjList.size() + 1);
		verletObjList.push_back(newObj);
    }
	
	void clearVerletObjects() // removes all balls from the simulation
	{
		for (VerletObject* obj : verletObjList)
		{
			delete obj;
		}
		verletObjList.clear();
		verletObjList.shrink_to_fit();
	}

	void update(float dt) // updates the simulation
	{
		const float sub_dt = dt / sub_steps;
		for (size_t i(sub_steps); i--;)
		{
			for (VerletObject* obj : verletObjList)
			{
				applyGravity(*obj);
				applyConstraint(*obj);
			}

			applyBallCollisions();

			for (VerletObject* obj : verletObjList)
			{
				obj->updatePosition(sub_dt);
			}
		}
	}

	void applyBallCollisions()
	{
		// first we need to setup and organize all the data for each of the balls
		verletScreenGrid.resetGridContent();
		for (VerletObject* obj : verletObjList)
		{
			verletScreenGrid.addVerletObjToGrid(obj);
		}


	}

	void applyGravity(VerletObject& obj) // applys gravity to a given object
	{
		obj.accelerate(this->gravity);
	}

	void applyConstraint(VerletObject& obj) // apply enviromental constraint, like the circle the balls sit inside
	{
		// Circular Constraint
		const sf::Vector2f position = (this->backgroundCircle.getPosition() - sf::Vector2f(obj_radius, obj_radius));
		const float radius = this->backgroundCircle.getRadius();

		const float objRad = obj.shape.getRadius();
		const sf::Vector2f v = position - obj.curPos;
		const float        dist = sqrt(v.x * v.x + v.y * v.y);
		if (dist > (radius - objRad)) {
			const sf::Vector2f n = v / dist;
			obj.curPos = position - n * (radius - objRad);
		}

		// TODO: Other Constraints?
	}

	void render(sf::RenderWindow* window)
	{
		window->draw(backgroundCircle);

		for (VerletObject* obj : verletObjList)
		{
			obj->render(window);
		}
	}
};