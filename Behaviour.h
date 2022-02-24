#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include "pch.h"

class Behaviour
{
public:
	Behaviour() {};
	virtual ~Behaviour() {};
	virtual void init(sf::RenderWindow* _window) {};
	virtual sf::Vector2f SteeringBehaviour() { return sf::Vector2f(0, 0);  };
	virtual void update(float dtime, sf::Event* event) = 0;
	virtual void render() = 0;
	virtual const char* getTitle() { return ""; };

};

#endif