#include "pch.h"
#include "Game.h"

#include "Seek.h"
#include "Flee.h"
#include "Pursue.h"
#include "Evade.h"
#include "Arrival.h"
#include "Wander.h"

#include "ObstacleAvoidance.h"

#include "Separate.h"
#include "Align.h"
#include "Cohere.h"
#include "Flock.h"

#include "FollowLeader.h"
#include "FollowLeader_Seek.h"
#include "FollowLeader_Flee.h"



int main() {
	// Init srand
	std::srand(static_cast<unsigned>(time(NULL)));

	bool quit = false;
	sf::Event event;

	Game* game = new Game();

	Behaviour* current_behaviour = new Seek;
	current_behaviour->init(game->getWindow()); // number is image number

	game->setWindowTitle(current_behaviour->getTitle());


	// Game Loop
	while (game->running() || !quit)
	{

		game->getWindow()->pollEvent(event); // poll event out here

		// Update and Render
		game->run(current_behaviour, &event); // polls event inside game

		game->getWindow()->pollEvent(event); // poll event out here

		switch (event.type)
		{
		case sf::Event::KeyPressed:

			if (event.key.code == sf::Keyboard::Num1)
			{

				delete(current_behaviour);
				current_behaviour = new Seek;
				current_behaviour->init(game->getWindow());
				game->setWindowTitle(current_behaviour->getTitle());
				std::cout << "User selected: " << current_behaviour->getTitle() << std::endl;

			}

			if (event.key.code == sf::Keyboard::Num2)
			{

				delete(current_behaviour);
				current_behaviour = new Flee;
				current_behaviour->init(game->getWindow());
				game->setWindowTitle(current_behaviour->getTitle());
				std::cout << "User selected: " << current_behaviour->getTitle() << std::endl;

			}

			if (event.key.code == sf::Keyboard::Num3)
			{
				delete(current_behaviour);
				current_behaviour = new Pursue;
				current_behaviour->init(game->getWindow());
				game->setWindowTitle(current_behaviour->getTitle());
				std::cout << "User selected: " << current_behaviour->getTitle() << std::endl;
			}

			if (event.key.code == sf::Keyboard::Num4)
			{
				delete(current_behaviour);
				current_behaviour = new Evade;
				current_behaviour->init(game->getWindow());
				game->setWindowTitle(current_behaviour->getTitle());
				std::cout << "User selected: " << current_behaviour->getTitle() << std::endl;
			}

			if (event.key.code == sf::Keyboard::Num5)
			{
				delete(current_behaviour);
				current_behaviour = new Arrival;
				current_behaviour->init(game->getWindow());
				game->setWindowTitle(current_behaviour->getTitle());
				std::cout << "User selected: " << current_behaviour->getTitle() << std::endl;
			}

			if (event.key.code == sf::Keyboard::Num6)
			{
				delete(current_behaviour);
				current_behaviour = new Wander;
				current_behaviour->init(game->getWindow());
				game->setWindowTitle(current_behaviour->getTitle());
				std::cout << "User selected: " << current_behaviour->getTitle() << std::endl;
			}

			if (event.key.code == sf::Keyboard::Num7)
			{
				delete(current_behaviour);
				current_behaviour = new ObstacleAvoidance;
				current_behaviour->init(game->getWindow());
				game->setWindowTitle(current_behaviour->getTitle());
				std::cout << "User selected: " << current_behaviour->getTitle() << std::endl;
			}

			if (event.key.code == sf::Keyboard::Num8)
			{
				delete(current_behaviour);
				current_behaviour = new Separate;
				current_behaviour->init(game->getWindow());
				game->setWindowTitle(current_behaviour->getTitle());
				std::cout << "User selected: " << current_behaviour->getTitle() << std::endl;
			}

			if (event.key.code == sf::Keyboard::Num9)
			{
				delete(current_behaviour);
				current_behaviour = new Align;
				current_behaviour->init(game->getWindow());
				game->setWindowTitle(current_behaviour->getTitle());
				std::cout << "User selected: " << current_behaviour->getTitle() << std::endl;
			}

			if (event.key.code == sf::Keyboard::Num0)
			{
				delete(current_behaviour);
				current_behaviour = new Cohere;
				current_behaviour->init(game->getWindow());
				game->setWindowTitle(current_behaviour->getTitle());
				std::cout << "User selected: " << current_behaviour->getTitle() << std::endl;
			}

			if (event.key.code == sf::Keyboard::Q)
			{
				delete(current_behaviour);
				current_behaviour = new Flock;
				current_behaviour->init(game->getWindow());
				game->setWindowTitle(current_behaviour->getTitle());
				std::cout << "User selected: " << current_behaviour->getTitle() << std::endl;
			}

			if (event.key.code == sf::Keyboard::W)
			{
				delete(current_behaviour);
				current_behaviour = new FollowLeader;
				current_behaviour->init(game->getWindow());
				game->setWindowTitle(current_behaviour->getTitle());
				std::cout << "User selected: " << current_behaviour->getTitle() << std::endl;
			}

			if (event.key.code == sf::Keyboard::E)
			{
				delete(current_behaviour);
				current_behaviour = new FollowLeader_Seek;
				current_behaviour->init(game->getWindow());
				game->setWindowTitle(current_behaviour->getTitle());
				std::cout << "User selected: " << current_behaviour->getTitle() << std::endl;
			}

			if (event.key.code == sf::Keyboard::R)
			{
				delete(current_behaviour);
				current_behaviour = new FollowLeader_Flee;
				current_behaviour->init(game->getWindow());
				game->setWindowTitle(current_behaviour->getTitle());
				std::cout << "User selected: " << current_behaviour->getTitle() << std::endl;
			}


			if (event.key.code == sf::Keyboard::Escape)
			{
				quit = true;
				game->closeWindow();
				std::cout << "User pressed : " << "Q" << std::endl;
			}

			break;

		case sf::Event::Closed: // window x button
			if (event.type == sf::Event::Closed)
				quit = true;
				game->closeWindow();

			break;
		}
	}



	return 0;

}