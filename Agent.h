#ifndef AGENT_H
#define AGENT_H


#include "pch.h"


class Agent
{

private:

	// render variables

	sf::RenderWindow* window = nullptr;

	// agent variables

	sf::Sprite agentSprite;
	sf::Texture agentTexture;
	int imageIdx; // which image 

	// Steering Variables

	sf::FloatRect targetArea;
	sf::Vector2f position;
	sf::Vector2f velocity;
	sf::Vector2f target;

	sf::RectangleShape traversableSquare;
	float mass;
	float orientation;
	float max_force;
	float max_velocity;

	float radius;


public:

	bool stopSpinning = false;;

	// Constructors and Destructors

	Agent();
	~Agent();


	// important functions for Agent class

	void init(sf::RenderWindow* window, int textureIdx, sf::Vector2f target);
	void render();
	void update(sf::Vector2f steering_force, float dtime);

	// other functions

	void setWindow(sf::RenderWindow* _window);
	bool BorderRule();
	void setPosition(sf::Vector2f _position);
	sf::Vector2f getSize();
	sf::Vector2f getLocalSize();
	void setTargetArea(float _x, float _y, float _w, float _h);
	void setTargetAreaByMargin(float _margin);
	void resetTargetArea();
	sf::FloatRect getTargetArea();
	sf::FloatRect fitsInBox(sf::FloatRect* _smallBox, sf::FloatRect* _bigBox);
	bool OutOfBounds(float _x, float _a, float _b);
	void Swap(float* _x, float* _y);
	void setTexture(int _imageIdx);
	void setTarget(sf::Vector2f _target);
	sf::Vector2f* getTarget();
	void setTargetToCenter();
	sf::Vector2f getRandomPosition(sf::FloatRect targetArea);
	float RandomRange(int min, int max);
	sf::Vector2f* getPosition();
	sf::Vector2f getVelocity();
	float& getMaxVelocity();
	float getMaxForce();
	float getMass();
	void setVelocity(sf::Vector2f _velocity);
	float RadianToDegree(float radian);
	sf::Vector2f Truncate(sf::Vector2f v, float max);
	float Magnitude(sf::Vector2f vector);
	sf::Vector2f Normalize(sf::Vector2f vector);
	void setTargetToRandomEdge();
	std::vector<sf::Vector2f> getTargetAreaVertices();
	int OppositeInt(int index, int range);
	sf::Vector2f RandPtBt2Pts(sf::Vector2f a, sf::Vector2f b);
	void setMaxVelocity(float _max_velocity);
	void setOrientation(float _orientation);
	float getOrientation();
	sf::Vector2f getOrigin();
	//void setMaxForce(float _maxForce);
	void setMass(float _mass);
	//float getOrientation();
	sf::Sprite* getSprite();
	//sf::Vector2f* getCenter();
	//sf::RenderWindow* getWindow();
	//float randomBinomial();
	//float Distance(sf::Vector2f a, sf::Vector2f b);

	void setRadius(float _radius);
	float getRadius();

	sf::Vector2f* getCenter();
	int getImageIdx();

};


#endif