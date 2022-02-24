#include "Game.h"

Game::Game()
{
	this->initVariables();

	this->initWindow();
}

Game::~Game()
{
	delete this->window;
}

void Game::initVariables()
{
	this->window = nullptr;
	
}

void Game::initWindow()
{
	this->videoMode.height = 1080;
	this->videoMode.width = 1920;

	// create window
	this->window = new sf::RenderWindow(this->videoMode, "Steering Behaviours", sf::Style::Titlebar | sf::Style::Close); //  | sf::Style::Fullscreen
	// set max frame rate
	this->window->setFramerateLimit(60);
	this->window->setKeyRepeatEnabled(false);

}

void Game::setWindowTitle(const char* title) { this->window->setTitle(title); }

const bool Game::running() { return this->window->isOpen(); }

sf::Event Game::pollEvents(Behaviour* behaviour, sf::Event* event)
{

	while (this->window->pollEvent(*event))
	{
		switch (event->type)
		{
		case sf::Event::Closed:
			this->window->close();
			break;
		case sf::Event::KeyPressed:
			if (event->key.code == sf::Keyboard::Escape)
			{
				this->window->close();
			}
			break;
		}
	}

	sf::Time timer = clock.restart(); // get elapsed time and restart timer
	float dtime = timer.asSeconds(); // delta time
	behaviour->update(dtime, event); //, &event

	return *event;
}

void Game::update(Behaviour* behaviour, sf::Event* event)
{ 
	updateMousePositions();
	pollEvents(behaviour, event);
}

void Game::render(Behaviour* behaviour)
{
	// clear previous frame
	this->window->clear(); // clear(sf::Color(0,255,0,255))

	// draw game objects
	behaviour->render();

	// display frame
	this->window->display();
}

void Game::run(Behaviour* behaviour, sf::Event* event)
{
	update(behaviour, event);

	render(behaviour);
}

void Game::closeWindow() { this->window->close(); }

sf::RenderWindow* Game::getWindow() { return this->window; }

void Game::updateMousePositions()
{
	this->mousePosWindow = sf::Mouse::getPosition(*this->window);
	this->mousePosView = this->window->mapPixelToCoords(this->mousePosWindow);
}