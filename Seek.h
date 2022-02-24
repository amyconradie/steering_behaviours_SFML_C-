#ifndef SEEK_H
#define SEEK_H

#include "pch.h"
#include "Behaviour.h"
#include "Agent.h"

class Seek :
	public Behaviour
{
private:

	std::vector<Agent*> agents;
	sf::Vector2f target;
	sf::RenderWindow* window;
	bool borderRule = true;

public:

	Seek();
	virtual ~Seek();
	virtual void init(sf::RenderWindow* _window);
	virtual void update(float dtime, sf::Event* event);
	virtual void render();
	virtual const char* getTitle();

	virtual sf::Vector2f SteeringBehaviour(Agent* agent);
	float Magnitude(sf::Vector2f vector);
	sf::Vector2f Normalize(sf::Vector2f vector);
	sf::Vector2f DesiredVelocity(sf::Vector2f current_position, sf::Vector2f target_position, float max_velocity);
	sf::Vector2f SteeringForce(sf::Vector2f desiredVelocity, sf::Vector2f velocity, float max_velocity, float max_force);

	float RandomRange(int min, int max);
	float OppositeFloat(float f);

};

#endif