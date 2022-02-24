
#ifndef GAME_H
#define GAME_H

#include "pch.h"
#include "Behaviour.h"

class Game
{
private:

	// Window
	sf::RenderWindow* window;
	sf::VideoMode videoMode;

	// Mouse Positions
	sf::Vector2i mousePosWindow;
	sf::Vector2f mousePosView;

	sf::Clock clock;

	void initVariables();
	void initWindow();


public:

	//constructors & destructors
	Game();
	~Game();

	// Accessors
	const bool running();

	//functions

	void setWindowTitle(const char* title);

	sf::Event pollEvents(Behaviour* behaviour, sf::Event* event);
	void update(Behaviour* behaviour, sf::Event* event);
	void render(Behaviour* behaviour);
	void run(Behaviour* behaviour, sf::Event* event);

	void closeWindow();

	sf::RenderWindow* getWindow();

	void updateMousePositions();
};

#endif