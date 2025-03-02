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

	// clear balls function
	auto clearBalls = [this](SquareButton* button) {
		this->physicsSystem.clearVerletObjects();
	};

    this->butManager.AddButton("Clear Balls", sf::Vector2f(5.f, 130.f), sf::Vector2f(100.f, 20.f), clearBalls, this->font);

	// grav set left
	auto gravLeft = [this](SquareButton* button) {
		this->physicsSystem.gravity.x -= 100.f;
	};

	this->butManager.AddButton("L", sf::Vector2f(20.f, 500.f), sf::Vector2f(20.f, 20.f), gravLeft, this->font);

	// grav set right
	auto gravRight = [this](SquareButton* button) {
		this->physicsSystem.gravity.x += 100.f;
		};

	this->butManager.AddButton(" R", sf::Vector2f(80.f, 500.f), sf::Vector2f(20.f, 20.f), gravRight, this->font);

	// grav set upwards
	auto gravUp = [this](SquareButton* button) {
		this->physicsSystem.gravity.y -= 100.f;
		};

	this->butManager.AddButton(" U", sf::Vector2f(50.f, 470.f), sf::Vector2f(20.f, 20.f), gravUp, this->font);

	// grav set downwards
	auto gravDown = [this](SquareButton* button) {
		this->physicsSystem.gravity.y += 100.f;
		};

	this->butManager.AddButton(" D", sf::Vector2f(50.f, 530.f), sf::Vector2f(20.f, 20.f), gravDown, this->font);

	auto gravReset = [this](SquareButton* button) {
		this->physicsSystem.gravity.x = 0.f;
		this->physicsSystem.gravity.y = 1000.f;
		};

	this->butManager.AddButton(" O", sf::Vector2f(50.f, 500.f), sf::Vector2f(20.f, 20.f), gravReset, this->font);
}
void Game::initResources()
{
	bool noResourceLoadIssues = true;

	noResourceLoadIssues = this->windowIcon.loadFromFile("Resources/Images/balls_icon.png");
	noResourceLoadIssues = this->font.loadFromFile("Resources/Fonts/pixelatedFont.ttf");

	if (!noResourceLoadIssues)
		std::cout << "Error loading resources!" << "\n";
}
void Game::initText()
{
	this->uiText.setFont(this->font);
	this->uiText.setCharacterSize(18);
	this->uiText.setFillColor(sf::Color::White);
	this->uiText.setString("N/A");
}
void Game::initWindow()
{
	this->videoMode.width = 640;
	this->videoMode.height = 480;
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
		// all this code determines if mouse clicks and to add a ball to the enviroment if it does
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
				for (size_t i = 0; i < 100; i++)
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

	ss << " Balls: " << this->physicsSystem.verletObjList.size() << "\n"
		<< " FPS: " << this->fps << "\n"
		<< " Grav:\n (" << this->physicsSystem.gravity.x << ", " << this->physicsSystem.gravity.y << ")\n";

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