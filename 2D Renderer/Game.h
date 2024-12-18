#pragma once

// STL includes
#include <chrono>
#include <iomanip>
#include <sstream>
#include <vector>
#include <ctime>
#include <iostream>

// SFML includes
#include <SFML/Graphics.hpp>

// Custom Includes
#include "PhysicsSolver.cpp"
#include "button_manager.h"

/*
* Primary Game Engine Wrapper Class
*/

class Game
{
	private:
		//// Private Functions & Variables ////

		/*
		* Windows
		*/
		sf::RenderWindow* window;
		sf::VideoMode videoMode;
		sf::Event ev;

		// Window FPS displaying
		std::chrono::steady_clock::time_point start, end, nextUpdate;
		std::chrono::milliseconds fpsUpdateInterval;
		bool displayFpsTitle;
		std::string fps;
		void calcFps();
		float smoothedFPS;

		/*
		* Initializers
		*/
		void initVariables();
		void initWindow();
		void initResources();
		void initText();

		/*
		* Updating
		*/

		// window updates
		void PollEvents();

		/*
		* Resources
		*/
		sf::Image windowIcon;
		sf::Font font;

		/*
		* Game Logic & Objects
		*/
		sf::Text uiText;

		std::chrono::steady_clock::time_point nextPhysicsUpdate;
		std::chrono::milliseconds physicsUpdateInterval;

		PhysSolver physicsSystem;
		button_manager butManager;

	public:
		// contstructors & deconstructors
		Game();
		~Game();

		// Accesors
		const bool getIsRunning()const;

		void setDisplayTitleFps(bool boolPar);
		void setFpsUpdateInterval(std::chrono::milliseconds msPar);

		// updates
		void update();

		// Rendering
		void render();
};