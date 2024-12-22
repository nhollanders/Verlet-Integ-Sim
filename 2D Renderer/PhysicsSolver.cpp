#pragma once
// std includes
#include <vector>
#include <iostream>

// SFML includes
#include <SFML/Graphics.hpp>

// custom includes
#include "VerletObject.cpp"
#include "util/math.h"

struct gridPos
{
	std::vector<VerletObject*> positionObjects;
};

struct PhysSolver
{
	// physics data
	sf::Vector2f gravity = {0.0f, 1000.0f};
	sf::CircleShape backgroundCircle;
	std::vector<VerletObject> verletObjects;

	// grid data
	sf::Vector2i gridSize;
	std::vector<std::vector<gridPos*>> dataGrid; // a 2d array that stores a list of verlet objects

	// phys objects data
	const int sub_steps = 4; // phys substeps
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

		// The size of the grid is just the radius of the collider divided by the size of the object.
		// Or size of the x or y of a box divided by the ball size if using a square or rectangle.
		gridSize.x = static_cast<int>(collider_radius / obj_radius);
		gridSize.y = static_cast<int>(collider_radius / obj_radius);

		dataGrid.resize(gridSize.x);

		for (int x = 0; x < gridSize.x; ++x)
		{
			dataGrid[x].resize(gridSize.y);
		}
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
			for (VerletObject& obj : verletObjects)
			{
				applyGravity(obj); // apply gravity
				applyConstraint(obj); // apply constraints on objects, in this instance its the outer bounds
			}

			updateCollisionGrid(); // setup the grid structure to store position of objects
			//setupCollisionFinding(); // setup and call functions to find the collision between objects

			updatePositions(sub_dt); // update all the object positions with the new position data determined
		}


		//std::cout << "Ball Count: " << this->verletObjects.size() << std::endl;
	}

    void updateCollisionGrid()
    {
        for (int x = 0; x < gridSize.x; ++x)
        {
            for (int y = 0; y < gridSize.y; ++y)
            {
                // Clear the grid position
                if (dataGrid[x][y] != nullptr)
                {
					dataGrid[x][y]->positionObjects.clear();
                }
            }
        }

        for (VerletObject& obj : verletObjects)
        {
            const sf::Vector2f oPos = obj.curPos;
            const float radius = obj.shape.getRadius();

            if (oPos.x > radius && oPos.x < gridSize.x - radius && // if position is more than the radius & position is less than the max grid size minus the radius
                oPos.y > radius && oPos.y < gridSize.y - radius) // same as top but in terms of y instead of x
            {
                int gridX = static_cast<int>(oPos.x / obj_radius);
                int gridY = static_cast<int>(oPos.y / obj_radius);
                if (dataGrid[gridX][gridY] == nullptr)
                {
                    dataGrid[gridX][gridY] = new gridPos();
                }
				dataGrid[gridX][gridY]->positionObjects.push_back(&obj);
            }
        }
    }

	void updatePositions(float dt)
	{
		for (VerletObject& obj : verletObjects)
		{
			obj.updatePosition(dt);
		}
	}

	void applyGravity(VerletObject& obj)
	{
		obj.accelerate(this->gravity);
	}

	void applyConstraint(VerletObject& obj)
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
		// TODO: Square Constraint
	}

	// solves a collisions between two objects
	void solveCollision(VerletObject& object_1, VerletObject& object_2)
	{
		const float response_coef = 0.75f;

		// Calculate the vector between the two objects
		sf::Vector2f v = object_1.curPos - object_2.curPos;
		float dist2 = v.x * v.x + v.y * v.y;
		float min_dist = object_1.shape.getRadius() + object_2.shape.getRadius();

		// Check if the objects are overlapping
		if (dist2 < min_dist * min_dist) {
			float dist = sqrt(dist2);
			sf::Vector2f n = v / dist; // Normalized vector

			// Calculate mass ratios based on object radii
			float mass_ratio_1 = object_2.shape.getRadius() / (object_1.shape.getRadius() + object_2.shape.getRadius());
			float mass_ratio_2 = object_1.shape.getRadius() / (object_1.shape.getRadius() + object_2.shape.getRadius());

			// Calculate the penetration depth and apply response coefficient
			float delta = 0.5f * response_coef * (dist - min_dist);

			// Update the positions to resolve collision
			object_1.curPos -= n * (mass_ratio_2 * delta);
			object_2.curPos += n * (mass_ratio_1 * delta);
		}
	}

	// determines if two objects are colliding or not
	void checkCollision(VerletObject& obj1, VerletObject& obj2)
	{
		float minDist = obj1.shape.getRadius() + obj2.shape.getRadius();
		float dist = EuclideanDist2DSqrt(obj1.curPos, obj2.curPos) < (minDist * minDist);

		// if collides
		if (dist < (obj1.shape.getRadius() * obj1.shape.getRadius()) + (obj2.shape.getRadius() * obj2.shape.getRadius()))
		{
			solveCollision(obj1, obj2); // solve collision
		}
	}
	
	void setupCollisionFinding()
	{
		//for (auto& obj : verletObjects)
		//{
		//	sf::Vector2i curObjPos = sf::Vector2i(static_cast<int>(obj.curPos.x), static_cast<int>(obj.curPos.y));
		//	GridPosition& gridPos = positionGrid.getGridPosition(curObjPos);
		//	auto posObjects = gridPos.getObjects(); // Ensure posObjects is a reference to avoid copying

		//	for (auto& subObjPtr : posObjects)
		//	{
		//		VerletObject& subObj = *subObjPtr;
		//		if (obj != subObj) // Avoid self-collision
		//		{
		//			checkCollision(obj, subObj);
		//		}
		//	}
		//}
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