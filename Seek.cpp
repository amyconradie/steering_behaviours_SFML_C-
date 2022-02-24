#include "Seek.h"

Seek::Seek()
{
	this->window = nullptr;
}

Seek::~Seek()
{
	for (auto& agent : agents)
	{
		delete agent;
	}
}

void Seek::init(sf::RenderWindow* _window) 
{
	Agent* agent = new Agent;
	this->window = _window;

	int imageIdx = (int)RandomRange(1, 7); // randomly choose image

	agent->init(this->window, imageIdx, agent->getRandomPosition(agent->getTargetArea()));
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

void Seek::update(float dtime, sf::Event* event)
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

	sf::Vector2f steering_force = SteeringBehaviour(agents[0]);

	agents[0]->update(-steering_force, dtime);

	this->target = *agents[0]->getTarget();

}

void Seek::render() 
{ 
	agents[1]->render(); // bg
	agents[0]->render(); // agent
	agents[2]->render(); // overlay
}

const char* Seek::getTitle() { return "Behaviour: Seek"; }
 

// Steering Behaviours

sf::Vector2f Seek::SteeringBehaviour(Agent* agent)
{
	sf::Vector2f tPos = *agent->getTarget();
	sf::Vector2f aPos = *agent->getPosition();
	sf::Vector2f v = agent->getVelocity();

	sf::Vector2f desiredVelocity = DesiredVelocity(tPos, aPos, agent->getMaxVelocity()); // move towards target

	if (!this->borderRule) {

		float l = agent->getTargetArea().left;
		float t = agent->getTargetArea().top;
		float w = agent->getTargetArea().width;
		float h = agent->getTargetArea().height;

		// out of bounds conditions

		float wallMargin = 0.f;

		bool oobX = aPos.x <= l + wallMargin;
		bool oobY = aPos.y <= t + wallMargin;
		bool oobW = aPos.x >= w - wallMargin;
		bool oobH = aPos.y >= h - wallMargin;

		// if agent x or y pos is out of bounds flip x or y velocity

		if (oobX || oobW) { v.x = OppositeFloat(v.x); }
		if (oobY || oobH) { v.y = OppositeFloat(v.y); }
	}

	sf::Vector2f steeringForce = SteeringForce(desiredVelocity, v, agent->getMaxVelocity(), agent->getMaxForce());

	return steeringForce;
}


// other functions

float Seek::OppositeFloat(float f) {
	float opposite;

	if (f < 0)
	{
		opposite = fabsf(f);
	}
	else if (f > 0){
		opposite = -fabsf(f);
	}
	else {
		opposite = 0;
	}

	return opposite;
}

float Seek::Magnitude(sf::Vector2f vector) 
{ 
	return (float)sqrt(vector.x * vector.x + vector.y * vector.y); 
}

sf::Vector2f Seek::Normalize(sf::Vector2f vector)
{
	float mag = Magnitude(vector);
	if (mag > 0.0f) {
		return vector / Magnitude(vector);
	}
	else {
		return sf::Vector2f();
	}
}

sf::Vector2f Seek::DesiredVelocity(sf::Vector2f current_position, sf::Vector2f target_position, float max_velocity)
{
	sf::Vector2f desiredVelocity = target_position - current_position;
	desiredVelocity = Normalize(desiredVelocity);
	desiredVelocity *= max_velocity;

	return desiredVelocity;
}

sf::Vector2f Seek::SteeringForce(sf::Vector2f desiredVelocity, sf::Vector2f velocity, float max_velocity, float max_force)
{
	sf::Vector2f steeringForce = desiredVelocity - velocity;
	steeringForce /= max_velocity;
	steeringForce *= max_force;

	return steeringForce;
}

float Seek::RandomRange(int min, int max) 
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
