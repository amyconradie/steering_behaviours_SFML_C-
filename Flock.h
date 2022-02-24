#ifndef FLOCK_H
#define FLOCK_H

#include "pch.h"
#include "Behaviour.h"
#include "Agent.h"

class Flock :
	public Behaviour
{

private:
	Agent* bg, * overlay;
	std::vector<Agent*> agents;
	sf::Vector2f lastClickPosition;
	sf::RenderWindow* window;
	bool borderRule = true;

	float MAX_AHEAD, MAX_AVOID_FORCE;

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


	Flock();

	~Flock();

	void init(sf::RenderWindow* _window);

	void CreateAgent(sf::Vector2f _spawnPoint);

	void update(float dtime, sf::Event* event);

	void render();

	const char* getTitle();

	sf::Vector2f SteeringBehaviour(Agent* agent);

	sf::Vector2f Separate(Agent* agent, float desiredSeparation);

	sf::Vector2f Align(Agent* agent, float neighbourDistance);

	sf::Vector2f Cohere(Agent* agent, float neighbourDistance);

	bool lineIntersectsAgent(sf::Vector2f ahead, sf::Vector2f ahead2, Agent* obstacle);

	float Distance(sf::Vector2f a, sf::Vector2f b);

	float RadiusOfCircumCircle(float w, float h);

	float OppositeFloat(float f);

	float Magnitude(sf::Vector2f vector);

	sf::Vector2f Normalize(sf::Vector2f vector);

	sf::Vector2f Truncate(sf::Vector2f v, float max);

	float RandomRange(int min, int max);

};



#endif