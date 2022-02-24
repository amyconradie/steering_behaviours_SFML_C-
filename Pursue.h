#ifndef PURSUE_H
#define PURSUE_H

#include "pch.h"
#include "Behaviour.h"
#include "Agent.h"

class Pursue :
	public Behaviour
{
private:

	std::vector<Agent*> agents;
	std::vector<Agent*> otherAgents;
	sf::Vector2f target;
	sf::Vector2f dist;
	sf::Vector2f objective;
	sf::RenderWindow* window;
	float t;
	bool borderRule = true;
	

public:

	Pursue();
	virtual ~Pursue();
	virtual void init(sf::RenderWindow* _window);
	virtual void update(float dtime, sf::Event* event);
	virtual void render();
	virtual const char* getTitle();

	virtual sf::Vector2f SteeringBehaviour(Agent* agent, Agent* prey);
	sf::Vector2f Seek(Agent* agent);
	float Magnitude(sf::Vector2f vector);
	sf::Vector2f Normalize(sf::Vector2f vector);
	sf::Vector2f DesiredVelocity(sf::Vector2f current_position, sf::Vector2f target_position, float max_velocity);
	sf::Vector2f SteeringForce(sf::Vector2f desiredVelocity, sf::Vector2f velocity, float max_velocity, float max_force);

	float RandomRange(int min, int max);
	float OppositeFloat(float f);

};

#endif