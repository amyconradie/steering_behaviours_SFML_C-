#include "Evade.h"

Evade::Evade()
{

	this->window = nullptr;

}

Evade::~Evade()
{
	for (auto& agent : agents)
	{
		delete agent;
	}

	for (auto& otherAgent : otherAgents)
	{
		delete otherAgent;
	}
}

void Evade::init(sf::RenderWindow* _window)
{
	this->window = _window;

	int imageIdx = (int)RandomRange(1, 7); // randomly choose image

	Agent* agent = new Agent;
	agent->init(this->window, imageIdx, agent->getRandomPosition(agent->getTargetArea()));
	agent->setPosition(agent->getRandomPosition(agent->getTargetArea()));
	this->target = *agent->getTarget();
	agents.push_back(agent);

	float old = imageIdx;
	while(imageIdx == old) // make sure two agents are different
	{
		imageIdx = (int)RandomRange(1, 7); // randomly choose image
	}

	Agent* otherAgent = new Agent;
	otherAgent->init(this->window, imageIdx, otherAgent->getRandomPosition(otherAgent->getTargetArea()));
	otherAgent->setPosition(agent->getRandomPosition(agent->getTargetArea()));
	otherAgent->setMass(0.1f);
	otherAgents.push_back(otherAgent);

	Agent* bg = new Agent;
	bg->init(this->window, 0, *bg->getPosition());
	agents.push_back(bg);

	Agent* overlay = new Agent;
	overlay->init(this->window, 9, *overlay->getPosition());
	agents.push_back(overlay);
}

void Evade::update(float dtime, sf::Event* event)
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

	agents[0]->BorderRule();
	sf::Vector2f steering_force = Seek(agents[0]);
	agents[0]->update(-steering_force, dtime);

	otherAgents[0]->BorderRule();
	steering_force = SteeringBehaviour(otherAgents[0], agents[0]); // Pursue, have otherAgent change agent
	otherAgents[0]->update(-steering_force, dtime);
}

void Evade::render()
{
	agents[1]->render(); // bg
	agents[0]->render(); // agent
	otherAgents[0]->render(); // otherAgent
	agents[2]->render(); // overlay
}

const char* Evade::getTitle() { return "Behaviour: Evade"; }


// Steering Behaviours

sf::Vector2f Evade::SteeringBehaviour(Agent* prey, Agent* predator)
{
	sf::Vector2f distance = *predator->getPosition() - *prey->getPosition(); // distance between prey and agent
	float updatesAhead = Magnitude(distance) / prey->getMaxVelocity(); // how many updates the target needs to move from its current position to the pursuer position
	sf::Vector2f futurePosition = *predator->getPosition() + predator->getVelocity() * updatesAhead;

	prey->setTarget(futurePosition);

	return Flee(prey);
}

sf::Vector2f Evade::Seek(Agent* agent)
{
	sf::Vector2f tPos = *agent->getTarget();
	sf::Vector2f aPos = *agent->getPosition();
	sf::Vector2f v = agent->getVelocity();

	sf::Vector2f desiredVelocity = DesiredVelocity(tPos, aPos, agent->getMaxVelocity()); // move towards target
	sf::Vector2f steeringForce = SteeringForce(desiredVelocity, v, agent->getMaxVelocity(), agent->getMaxForce());

	return steeringForce;
}

sf::Vector2f Evade::Flee(Agent* agent)
{
	sf::Vector2f tPos = *agent->getTarget();
	sf::Vector2f aPos = *agent->getPosition();
	sf::Vector2f size = agent->getSize();
	sf::Vector2f v = agent->getVelocity();
	sf::Vector2f desiredVelocity; // move away from target


	// check that agent wont constantly back into the walls to get away from the target
	if (agents[0]->BorderRule()) {
		*agents[0]->getTarget() = *agents[0]->getPosition();
	}

	desiredVelocity = DesiredVelocity(aPos, tPos, agent->getMaxVelocity()); // move away from target
	

	sf::Vector2f steeringForce = SteeringForce(desiredVelocity, v, agent->getMaxVelocity(), agent->getMaxForce());

	return steeringForce;
}


// Other Functions

float Evade::OppositeFloat(float f) {
	float opposite;

	if (f < 0) { opposite = fabsf(f); }
	else if (f > 0) { opposite = -fabsf(f); }
	else { opposite = 0; }

	return opposite;
}

float Evade::Magnitude(sf::Vector2f vector)
{
	return (float)sqrt(vector.x * vector.x + vector.y * vector.y);
}

sf::Vector2f Evade::Normalize(sf::Vector2f vector)
{
	float mag = Magnitude(vector);
	if (mag > 0.0f) { return vector / Magnitude(vector); }
	else { return sf::Vector2f(); }
}

sf::Vector2f Evade::DesiredVelocity(sf::Vector2f current_position, sf::Vector2f target_position, float max_velocity)
{
	sf::Vector2f desiredVelocity = target_position - current_position;
	desiredVelocity = Normalize(desiredVelocity);
	desiredVelocity *= max_velocity;

	return desiredVelocity;
}

sf::Vector2f Evade::SteeringForce(sf::Vector2f desiredVelocity, sf::Vector2f velocity, float max_velocity, float max_force)
{
	sf::Vector2f steeringForce = desiredVelocity - velocity;
	steeringForce /= max_velocity;
	steeringForce *= max_force;

	return steeringForce;
}

float Evade::RandomRange(int min, int max)
{
	int n = max - min + 1;
	int remainder = RAND_MAX % n;
	int x;

	do { x = rand(); } while (x >= RAND_MAX - remainder);

	float random = static_cast<float>(min + x % n);
	return random;
}