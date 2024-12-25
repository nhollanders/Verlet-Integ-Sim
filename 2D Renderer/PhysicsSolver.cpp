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

struct VerletObjectPos
{
	VerletObject* VObj;
	sf::Vector2i VObjPos;
};

struct PhysSolver
{
	// physics data
	sf::Vector2f gravity = {0.0f, 1000.0f};
	sf::CircleShape backgroundCircle;
	std::vector<VerletObjectPos*> verletObjects;

	// grid data
	sf::Vector2i gridSize;
	std::vector<std::vector<gridPos*>> dataGrid; // a 2d array that stores a list of verlet objects

	// phys objects data
	const int sub_steps = 8; // phys substeps
	const float obj_radius = 2.f; // radius of the balls
	const float collider_radius = 300.f; // radius of the collider
	const sf::Vector2f collider_pos = sf::Vector2f(400.f, 300.f);

	PhysSolver()
	{
		sf::CircleShape backgroundCirclet{ collider_radius };
		backgroundCirclet.setOrigin(collider_radius, collider_radius);
		backgroundCirclet.setFillColor(sf::Color::White);
		backgroundCirclet.setRadius(collider_radius);
		backgroundCirclet.setPosition(collider_pos);
		backgroundCirclet.setPointCount(128);

		this->backgroundCircle = backgroundCirclet;

		// The size of the grid is just the radius of the collider divided by the size of the object.
		// Or size of the x or y of a box divided by the ball size if using a square or rectangle.
		gridSize.x = static_cast<int>((collider_radius*2) / obj_radius); // so that balls will be within grid cells, lower resolution means less cells while higher means more
		gridSize.y = static_cast<int>((collider_radius*2) / obj_radius);

		dataGrid.resize(gridSize.x);
		for (size_t x = 0; x < dataGrid.size(); x++)
		{
			dataGrid[x].resize(gridSize.y);
			for (size_t y = 0; y < dataGrid[x].size(); y++)
			{
				dataGrid[x][y] = new gridPos(); // Allocate memory for each gridPos
				dataGrid[x][y]->positionObjects.resize(2);
			}
		}
	}

    void addVerletObject(sf::Vector2f pos)
    {
        VerletObject* newObject = new VerletObject(pos, obj_radius, verletObjects.size() + 1);
        VerletObjectPos* newPos = new VerletObjectPos{ newObject };
        verletObjects.push_back(newPos);
    }

	void update(float dt)
	{
		const float sub_dt = dt / static_cast<float>(sub_steps);
		for (size_t i(sub_steps); i--;)
		{
			for (VerletObjectPos* objPos : verletObjects)
			{
				applyGravity(*objPos->VObj); // apply gravity
				applyConstraint(*objPos->VObj); // apply constraints on objects, in this instance its the outer bounds
			}

			updateCollisionGrid(); // setup the grid structure to store position of objects
			setupCollisionFinding(); // setup and call functions to find the collision between objects

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
				dataGrid[x][y]->positionObjects.clear();
			}
		}

		for (VerletObjectPos* objPos : verletObjects)
		{
			const sf::Vector2f oPos = objPos->VObj->curPos;
			const float radius = objPos->VObj->shape.getRadius();

			int gridX = static_cast<int>((oPos.x - (collider_pos.x - collider_radius)) / obj_radius);
			int gridY = static_cast<int>((oPos.y - (collider_pos.y - collider_radius)) / obj_radius);

			// Ensure the grid position is within bounds
			if (gridX >= 0 && gridX < gridSize.x && gridY >= 0 && gridY < gridSize.y)
			{
				dataGrid[gridX][gridY]->positionObjects.push_back(objPos->VObj);
				objPos->VObjPos = { gridX, gridY };
			}
		}
	}

	// solves a collisions between two objects
	void solveCollision(VerletObject& object_1, VerletObject& object_2)
	{
		constexpr float response_coef = 1.0f;
		constexpr float eps = 0.0001f;

		// Calculate the vector between the two objects
		sf::Vector2f v = object_1.curPos - object_2.curPos;
		float dist2 = v.x * v.x + v.y * v.y;
		float min_dist = object_1.shape.getRadius() + object_2.shape.getRadius();

		// Check if the objects are overlapping
		if (dist2 < min_dist * min_dist && dist2 > eps) {
			float dist = sqrt(dist2);
			sf::Vector2f n = v / dist; // Normalized vector

			// Calculate the penetration depth and apply response coefficient
			float delta = response_coef * 0.5f * (min_dist - dist);
			sf::Vector2f correction_vector = n * delta;

			// Update the positions to resolve collision
			object_1.curPos += correction_vector;
			object_2.curPos -= correction_vector;
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
			//std::cout << "Collision: (" << obj1.curPos.x << "," << obj1.curPos.y << ")|(" << obj2.curPos.x << "," << obj2.curPos.y << ")\n";
		}
	}
	
	void setupCollisionFinding()
	{
		for (VerletObjectPos* objPos : verletObjects)
		{
			const int curX = objPos->VObjPos.x;
			const int curY = objPos->VObjPos.y;

			VerletObject* curObj = objPos->VObj;

			// Check the 8 surrounding cells including the current cell
			for (int offsetX = -1; offsetX <= 1; ++offsetX)
			{
				for (int offsetY = -1; offsetY <= 1; ++offsetY)
				{
					int neighborX = curX + offsetX;
					int neighborY = curY + offsetY;

					// Ensure the neighbor cell is within grid bounds
					if (neighborX >= 0 && neighborX < gridSize.x && neighborY >= 0 && neighborY < gridSize.y)
					{
						for (VerletObject* neighborObj : dataGrid[neighborX][neighborY]->positionObjects)
						{
							if (neighborObj != nullptr && neighborObj != curObj)
							{
								checkCollision(*curObj, *neighborObj);
							}
						}
					}
				}
			}
		}
	}

	void updatePositions(float dt)
	{
		for (VerletObjectPos* objPos : verletObjects)
		{
			objPos->VObj->updatePosition(dt);
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

	void render(sf::RenderWindow* window)
	{
		window->draw(backgroundCircle);

		for (VerletObjectPos* objPos : verletObjects)
		{
			objPos->VObj->render(window);
		}
	}
};