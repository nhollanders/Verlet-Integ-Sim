#include "Game.h"

using namespace sf;
using namespace std::chrono;

/*
* Initializers
*/
void Game::initVariables()
{
	this->window = nullptr;

	// FPS title displaying
	this->smoothedFPS = 0.5f;
	this->start = steady_clock::now();
	this->displayFpsTitle = false;
	this->fpsUpdateInterval = std::chrono::milliseconds(100);

	// Game logic
	this->physicsUpdateInterval = std::chrono::milliseconds(40);
	this->nextPhysicsUpdate = std::chrono::steady_clock().now() + this->physicsUpdateInterval;
	this->fps = "N/A";

	SquareButton* newBut1 = this->butManager.AddButton("Test", sf::Vector2f(20.f, 20.f), this->font);
	newBut1->buttonPressed = [this]()
	{
		std::cout << "test press 1";
	};
}
void Game::initResources()
{
	bool noResourceLoadIssues = true;

	noResourceLoadIssues = this->windowIcon.loadFromFile("Resources/Images/balls_icon.png");
	noResourceLoadIssues = this->font.loadFromFile("Resources/Fonts/PolygonParty.ttf");

	if (!noResourceLoadIssues)
		std::cout << "Error loading resources!" << "\n";
}
void Game::initText()
{
	this->uiText.setFont(this->font);
	this->uiText.setCharacterSize(24);
	this->uiText.setFillColor(sf::Color::White);
	this->uiText.setString("N/A");
}
void Game::initWindow()
{
	this->videoMode.height = 480;
	this->videoMode.width = 640;
	this->window = new RenderWindow(VideoMode(800, 600), "Ball Simulator", Style::Titlebar | Style::Close | Style::Resize);
	this->window->setIcon(this->windowIcon.getSize().x, this->windowIcon.getSize().y, this->windowIcon.getPixelsPtr());
}

void Game::PollEvents()
{
	while (this->window->pollEvent(this->ev))
	{
		switch (this->ev.type)
		{
		case Event::Closed:
			this->window->close();
			break;

		case Event::KeyPressed:
			if (this->ev.key.code == Keyboard::Escape)
			{
				this->window->close();
				break;
			}
		}
	}
}

void Game::calcFps() // ran right after rendering
{

	// FPS window title displaying
	this->end = steady_clock::now();
	if (this->nextUpdate <= this->end) // Update only at intervals
	{
		// Calculate frame duration in seconds
		float frameDurationSec = std::chrono::duration<float>(this->end - this->start).count();

		// Avoid division by zero
		if (frameDurationSec > 0.0f) // really should never reach 0 since thats just perfect frame duration so itll skip that frame of calc
		{
			float currentFPS = 1.0f / frameDurationSec;

			// Initialize smoothedFPS if not already done
			if (this->smoothedFPS < 0.0f) // Assuming smoothedFPS is initialized to -1 or an invalid FPS
			{
				this->smoothedFPS = currentFPS;
			}

			// Apply exponential moving average (EMA) smoothing
			constexpr float alpha = 0.1f; // Smoothing factor (adjust as needed)
			this->smoothedFPS = alpha * currentFPS + (1.0f - alpha) * this->smoothedFPS;

			// Format smoothed FPS
			std::stringstream stream;
			stream << std::fixed << std::setprecision(2) << this->smoothedFPS; // Precision to 2 decimal places
			std::string fpsString = stream.str();

			this->fps = fpsString;

			if (this->displayFpsTitle)
			{
				this->window->setTitle("FPS: " + fpsString);
			}
		}

		// Set the next update time
		this->nextUpdate = this->end + this->fpsUpdateInterval;
	}

	// Update start time for next frame
	this->start = steady_clock::now();
}

/*
* Constructors
*/
Game::Game()
{
	this->initVariables(); // initialize all variables
	this->initResources(); // initializes resources like textures
	this->initWindow(); // initialize window
	this->initText();
}

Game::~Game()
{
	delete this->window;
}

/*
* Accessors
*/
const bool Game::getIsRunning() const {
	return this->window->isOpen();
}
void Game::setDisplayTitleFps(bool boolPar) { // lets you set if the fps is displayed in the title or not
	this->displayFpsTitle = boolPar;
}
void Game::setFpsUpdateInterval(std::chrono::milliseconds msPar) { // lets you set how often the fps changes in the title in milliseconds
	this->fpsUpdateInterval = msPar;
}

/*
* Public functions
*/

void Game::update() // game logic and functionality
{
	this->PollEvents();
    this->butManager.update(*this->window);

	if (this->nextPhysicsUpdate <= std::chrono::steady_clock().now())
	{
		sf::Vector2f mousePos = this->window->mapPixelToCoords(sf::Mouse::getPosition(*this->window));
		float eqX = mousePos.x - this->physicsSystem.backgroundCircle.getPosition().x;
		float eqY = mousePos.y - this->physicsSystem.backgroundCircle.getPosition().y;
		float dist = (eqX * eqX) + (eqY * eqY);
		float maxDist = this->physicsSystem.collider_radius * this->physicsSystem.collider_radius;

		if (dist <= maxDist)
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
			{
				this->physicsSystem.addVerletObject(mousePos);
			}

			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
			{
				for (size_t i = 0; i < 10; i++)
				{
					this->physicsSystem.addVerletObject(mousePos + sf::Vector2f(static_cast<float>(i*2), 0.f));
				}
			}
		}


		float dt = 1.f/30.f;

		this->physicsSystem.update(dt);

		this->nextPhysicsUpdate = std::chrono::steady_clock().now() + this->physicsUpdateInterval;
	};

	std::stringstream ss;

	ss << " Balls: " << this->physicsSystem.verletObjects.size() << "\n"
	   << " FPS: " << this->fps << "\n";

	this->uiText.setString(ss.str());
}

void Game::render() // rendering pixels on screen
{
	/*
	* Renders the game in order:
	* - clear window with color
	* - draw objects
	* - display window
	*/

	this->window->clear();

	// render here
	this->physicsSystem.render(this->window);
    this->butManager.render(*this->window);
	this->window->draw(this->uiText);

	this->window->display();
	this->calcFps();
}