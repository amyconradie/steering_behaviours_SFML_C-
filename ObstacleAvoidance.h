#ifndef OBSTACLEAVOIDANCE_H
#define OBSTACLEAVOIDANCE_H

#include "pch.h"
#include "Behaviour.h"
#include "Agent.h"
class ObstacleAvoidance :
    public Behaviour
{

private:
	Agent* bg, *overlay;
	std::vector<Agent*> agents;
	std::vector<Agent*> obstacles;
	sf::Vector2f lastClicked;
	sf::Vector2f target;
	sf::RenderWindow* window;
	bool borderRule = true;

	float MAX_AHEAD, MAX_AVOID_FORCE;

public:


	ObstacleAvoidance();

	~ObstacleAvoidance();

	void init(sf::RenderWindow* _window);

	void CreateObstacle(sf::Vector2f _spawnPoint);

	void CreateAgent(sf::Vector2f _spawnPoint);

	void update(float dtime, sf::Event* event);

	void render();

	const char* getTitle();

	sf::Vector2f SteeringBehaviour(Agent* agent);

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