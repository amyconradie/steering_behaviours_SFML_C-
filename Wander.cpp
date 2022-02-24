#include "Wander.h"

Wander::Wander() : target(sf::Vector2f(640, 360)),
				   wanderRadius(150),
				   wanderOffset(200),
				   wanderMaxAngle(60),
				   randTarget(sf::Vector2f(rand() % 1280, rand() % 768)),
				   angle(0)
{
	this->window = nullptr;
}

Wander::~Wander()
{
	for (auto& agent : agents)
	{
		delete agent;
	}
}

void Wander::init(sf::RenderWindow* _window)
{
	Agent* agent = new Agent;
	this->window = _window;

	int imageIdx = (int)RandomRange(1, 7); // randomly choose image

	agent->init(this->window, imageIdx, agent->getRandomPosition(agent->getTargetArea()));
	agent->setPosition(agent->getRandomPosition(agent->getTargetArea()));
	agent->setMass(0.02f);
	agents.push_back(agent);

	Agent* bg = new Agent;
	bg->init(this->window, 0, *bg->getPosition());
	agents.push_back(bg);

	Agent* overlay = new Agent;
	overlay->init(this->window, 9, *overlay->getPosition());
	agents.push_back(overlay);
}

void Wander::update(float dtime, sf::Event* event)
{
	agents[0]->BorderRule();

	sf::Vector2f dist = *agents[0]->getTarget() - *agents[0]->getPosition();
	this->angle = float(rand() % 360);

	this->centre = *agents[0]->getPosition() + agents[0]->Normalize(agents[0]->getVelocity()) * this->wanderOffset;

	target.x = centre.x + wanderRadius * sin(angle);
	target.y = centre.y + wanderRadius * cos(angle);

	agents[0]->setTarget(this->target);
	sf::Vector2f steering_force = SteeringBehaviour(agents[0]);
	agents[0]->update(-steering_force, dtime);
}

void Wander::render()
{
	agents[1]->render(); // bg
	agents[0]->render(); // agent
	agents[2]->render(); // overlay
}

const char* Wander::getTitle() { return "Behaviour: Wander"; }


// Steering Behaviours

sf::Vector2f Wander::SteeringBehaviour(Agent* agent)
{
	sf::Vector2f desiredVelocity = DesiredVelocity(*agent->getTarget(), *agent->getPosition(), agent->getMaxVelocity()); // move towards target
	sf::Vector2f steeringForce = SteeringForce(desiredVelocity, agent->getVelocity(), agent->getMaxVelocity(), agent->getMaxForce());

	return steeringForce;
}


float Wander::Magnitude(sf::Vector2f vector)
{
	return (float)sqrt(vector.x * vector.x + vector.y * vector.y);
}

sf::Vector2f Wander::Normalize(sf::Vector2f vector)
{
	float mag = Magnitude(vector);
	if (mag > 0.0f) {
		return vector / Magnitude(vector);
	}
	else {
		return sf::Vector2f();
	}
}

sf::Vector2f Wander::DesiredVelocity(sf::Vector2f current_position, sf::Vector2f target_position, float max_velocity)
{
	sf::Vector2f desiredVelocity = target_position - current_position;
	desiredVelocity = Normalize(desiredVelocity);
	desiredVelocity *= max_velocity;

	return desiredVelocity;
}

sf::Vector2f Wander::SteeringForce(sf::Vector2f desiredVelocity, sf::Vector2f velocity, float max_velocity, float max_force)
{
	sf::Vector2f steeringForce = desiredVelocity - velocity;
	steeringForce /= max_velocity;
	steeringForce *= max_force;

	return steeringForce;
}

float Wander::RandomRange(int min, int max)
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
