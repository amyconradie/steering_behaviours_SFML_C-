#include "Arrival.h"

Arrival::Arrival()
{
	this->window = nullptr;
}

Arrival::~Arrival()
{
	for (auto& agent : agents)
	{
		delete agent;
	}
}

void Arrival::init(sf::RenderWindow* _window)
{
	this->window = _window;
	slowingRadius = 20.f;

	int imageIdx = (int)RandomRange(1, 7); // randomly choose image

	Agent* agent = new Agent;
	agent->init(this->window, imageIdx, agent->getRandomPosition(agent->getTargetArea()));
	agent->setMaxVelocity(500);
	agent->setPosition(agent->getRandomPosition(agent->getTargetArea()));
	this->target = *agent->getTarget();
	agents.push_back(agent);

	Agent* bg = new Agent;
	bg->init(this->window, 0, *bg->getPosition());
	agents.push_back(bg);

	Agent* overlay = new Agent;
	overlay->init(this->window, 9, *overlay->getPosition());
	agents.push_back(overlay);
}

void Arrival::update(float dtime, sf::Event* event)
{
	switch (event->type) {
	case sf::Event::MouseButtonReleased:
		if (sf::Event::MouseButtonReleased && event->mouseButton.button == sf::Mouse::Left)
		{
			sf::Vector2f target = sf::Vector2f(
				(float)(sf::Mouse::getPosition(*this->window).x),
				(float)(sf::Mouse::getPosition(*this->window).y)
			);

			if (target != *agents[0]->getTarget()) {
				agents[0]->setTarget(target);
				std::cout << "Mouse Clicked at: x:" << target.x << ", y: " << target.y << std::endl;
			}
		}
		break;
	default:
		break;
	}

	//agents[0]->BorderRule();

	sf::Vector2f steering_force = SteeringBehaviour(agents[0]);
	agents[0]->update(steering_force, dtime);

	this->target = *agents[0]->getTarget();
}

void Arrival::render() 
{
	agents[1]->render(); // bg
	agents[0]->render(); // agent
	agents[2]->render(); // overlay
}

const char* Arrival::getTitle() { return "Behaviour: Arrival"; }
 

// Steering Behaviours

sf::Vector2f Arrival::SteeringBehaviour(Agent* agent) {
	sf::Vector2f desiredV = target - *agent->getPosition();
	sf::Vector2f steeringForce = desiredV - agent->getVelocity();
	sf::Vector2f dist = target - *agent->getPosition();

	if (Magnitude(dist) >= slowingRadius) {
		desiredV = Normalize(desiredV);
		desiredV *= agent->getMaxVelocity();
		steeringForce /= agent->getMaxVelocity();
		steeringForce *= agent->getMaxForce();
	}
	else {
		desiredV *= 2*Magnitude(desiredV);
		desiredV *= Magnitude(dist) / slowingRadius;
	}

	return steeringForce;
}

sf::Vector2f Arrival::DesiredVelocity(sf::Vector2f current_position, sf::Vector2f target_position, float max_velocity)
{
	sf::Vector2f desiredVelocity = target_position - current_position;
	desiredVelocity = Normalize(desiredVelocity);
	desiredVelocity *= max_velocity;

	return desiredVelocity;
}

sf::Vector2f Arrival::SteeringForce(sf::Vector2f desiredVelocity, sf::Vector2f velocity, float max_velocity, float max_force)
{
	sf::Vector2f steeringForce = desiredVelocity - velocity;
	steeringForce /= max_velocity;
	steeringForce *= max_force;

	return steeringForce;
}


// Other Functions

float Arrival::Magnitude(sf::Vector2f vector)
{
	return (float)sqrt((vector.x * vector.x) + (vector.y * vector.y));
}

sf::Vector2f Arrival::Normalize(sf::Vector2f vector)
{
	float mag = Magnitude(vector);
	if (mag > 0.0f) {
		return vector / Magnitude(vector);
	}
	else {
		return sf::Vector2f();
	}
}

float Arrival::RandomRange(int min, int max)
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
