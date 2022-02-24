#include "Separate.h"

Separate::Separate()
{

	this->window = nullptr;
}

Separate::~Separate()
{
	delete bg;
	delete overlay;

	for (auto& agent : agents)
	{
		delete agent;
	}
}

void Separate::init(sf::RenderWindow* _window)
{
	this->window = _window;

	this->MAX_AHEAD = 200;
	this->MAX_AVOID_FORCE = 200;

	this->bg = new Agent;
	bg->init(this->window, 0, *bg->getPosition());

	this->overlay = new Agent;
	overlay->init(this->window, 9, *overlay->getPosition());

	this->wanderRadius = 150;
	this->wanderOffset = 200;
	this->wanderMaxAngle = 60;
	this->randTarget = sf::Vector2f(rand() % 1280, rand() % 768);
	this->angle = 0;
}

void Separate::CreateAgent(sf::Vector2f _spawnPoint)
{
	Agent* agent = new Agent;
	int imageIdx = (int)RandomRange(1, 7); // randomly choose image
	agent->init(this->window, imageIdx, _spawnPoint);
	agent->setPosition(_spawnPoint);
	agent->setTarget(sf::Vector2f(640, 360));
	agent->setRadius(50.f);
	agents.push_back(agent);
}

void Separate::update(float dtime, sf::Event* event)
{
	switch (event->type) {
	case sf::Event::MouseButtonReleased:
		if (sf::Event::MouseButtonReleased && event->mouseButton.button == sf::Mouse::Right) // Right click create agent
		{
			sf::Vector2f spawnPoint = sf::Vector2f(
				(float)(sf::Mouse::getPosition(*this->window).x),
				(float)(sf::Mouse::getPosition(*this->window).y)
			);

			if (this->lastClickPosition != spawnPoint) {
				CreateAgent(spawnPoint);
				this->lastClickPosition = spawnPoint;
			}
		}
		break;
	default:
		break;
	}

	for (auto& agent : agents) // if no obstacles
	{
		agent->BorderRule();

		sf::Vector2f dist = *agent->getTarget() - *agent->getPosition();
		this->angle = float(rand() % 360);

		this->centre = *agent->getPosition() + agent->Normalize(agent->getVelocity()) * this->wanderOffset;

		sf::Vector2f target;
		target.x = this->centre.x + this->wanderRadius * sin(this->angle);
		target.y = this->centre.y + this->wanderRadius * cos(this->angle);

		agent->setTarget(target);

		sf::Vector2f steering_force = SteeringBehaviour(agent);

		agent->update(steering_force, dtime);
	}
}

void Separate::render()
{
	this->bg->render(); // bg

	for (auto& agent : agents)
	{
		agent->render();
	}

	this->overlay->render(); // overlay
}

const char* Separate::getTitle() { return "Behaviour: Separate - Wander"; }


// Steering Behaviours

sf::Vector2f Separate::SteeringBehaviour(Agent* agent)
{
	float radius = RadiusOfCircumCircle(agent->getSize().x, agent->getSize().y);
	float desiredSeparation = radius * 2.f;

	sf::Vector2f desiredVelocity = sf::Vector2f(0, 0);
	int count = 0;

	for (auto& other : agents)
	{
		float distance = Distance(*agent->getPosition(), *other->getPosition());

		// check if testing against self

		bool self = (agent->getPosition() == other->getPosition());
		bool neighbour = (distance >= 0 && distance < desiredSeparation);

		if (neighbour && !self)
		{
			sf::Vector2f difference = *agent->getPosition() - *other->getPosition();
			difference = Normalize(difference);
			difference /= distance;
			desiredVelocity += difference;
			count++;
		}
	}

	sf::Vector2f steeringForce = sf::Vector2f(0, 0);

	if (count > 0) // if the agent has neighbours
	{
		desiredVelocity /= (float)count;
		desiredVelocity = Normalize(desiredVelocity);
		desiredVelocity *= agent->getMaxVelocity();
		steeringForce = desiredVelocity - agent->getVelocity();
		steeringForce /= agent->getMaxVelocity();
		steeringForce *= agent->getMaxForce();
	}
	else // if agent has no neighbours
	{
		desiredVelocity = *agent->getTarget() - *agent->getPosition();
		desiredVelocity = Normalize(desiredVelocity);
		desiredVelocity *= agent->getMaxVelocity();

		steeringForce = desiredVelocity - agent->getVelocity();
		steeringForce /= agent->getMaxVelocity();
		steeringForce *= agent->getMaxForce();
	}

	return steeringForce;

}




// other functions

bool Separate::lineIntersectsAgent(sf::Vector2f ahead, sf::Vector2f ahead2, Agent* obstacle)
{
	std::cout << "radius: x: " << RadiusOfCircumCircle(obstacle->getLocalSize().x, obstacle->getLocalSize().y) << std::endl;
	return Distance(*obstacle->getCenter(), ahead) <= RadiusOfCircumCircle(obstacle->getLocalSize().x, obstacle->getLocalSize().y) || Distance(*obstacle->getCenter(), ahead2) <= RadiusOfCircumCircle(obstacle->getLocalSize().x, obstacle->getLocalSize().y);
}

float Separate::Distance(sf::Vector2f a, sf::Vector2f b)
{
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

float Separate::RadiusOfCircumCircle(float w, float h)
{
	// the sides cannot be negative
	if (w < 0 || h < 0)
		return -1;

	// Radius of the circumcircle
	float radius = sqrt(pow(w, 2) + pow(h, 2)) / 2;

	// Return the radius
	return radius;
}

sf::Vector2f Separate::Truncate(sf::Vector2f v, float max)
{
	sf::Vector2f truncated = v;
	if (Magnitude(v) > max)
	{
		truncated = Normalize(truncated) * max;
	}

	return truncated;
}

float Separate::OppositeFloat(float f) {
	float opposite;

	if (f < 0)
	{
		opposite = fabsf(f);
	}
	else if (f > 0) {
		opposite = -fabsf(f);
	}
	else {
		opposite = 0;
	}

	return opposite;
}

float Separate::Magnitude(sf::Vector2f vector)
{
	return (float)sqrt(vector.x * vector.x + vector.y * vector.y);
}

sf::Vector2f Separate::Normalize(sf::Vector2f vector)
{
	float mag = Magnitude(vector);
	if (mag > 0.0f) {
		return vector / Magnitude(vector);
	}
	else {
		return sf::Vector2f();
	}
}

float Separate::RandomRange(int min, int max)
{
	int n = max - min + 1;
	int remainder = RAND_MAX % n;
	int x;

	do {
		x = rand();
	} while (x >= RAND_MAX - remainder);

	float random = static_cast<float>(min + x % n);
	return random;
}
