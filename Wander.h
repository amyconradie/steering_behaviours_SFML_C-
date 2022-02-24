#ifndef WANDER_H
#define WANDER_H

#include "pch.h"
#include "Behaviour.h"
#include "Agent.h"

class Wander :
	public Behaviour
{
private:

	std::vector<Agent*> agents;
	sf::Vector2f target;
	sf::RenderWindow* window;
	bool borderRule = true;

	// random position wander
	int newDecision;
	float wanderTimer;

	// random angle wander
	sf::Vector2f centre;
	sf::Vector2f randTarget;
	float orientation = 0.f;
	float wanderAngle = 0.f; 
	float wanderMaxAngle = 0.f; 
	float wanderOffset = 0.f; 
	float wanderRadius = 0.f;
	float targetAngle = 0.f; 
	float angle = 0.f;

public:

	Wander();
	virtual ~Wander();
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

};

#endif