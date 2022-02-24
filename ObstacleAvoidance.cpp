#include "ObstacleAvoidance.h"

ObstacleAvoidance::ObstacleAvoidance()
{

	this->window = nullptr;
}

ObstacleAvoidance::~ObstacleAvoidance()
{
	delete bg;

	delete overlay;

	for (auto& agent : agents)
	{
		delete agent;
	}

	for (auto& obstacle : obstacles)
	{
		delete obstacle;
	}
}

void ObstacleAvoidance::init(sf::RenderWindow* _window)
{
	this->window = _window;

	this->MAX_AHEAD = 200;
	this->MAX_AVOID_FORCE = 200;

	this->bg = new Agent;
	bg->init(this->window, 0, *bg->getPosition());

	this->overlay = new Agent;
	overlay->init(this->window, 9, *overlay->getPosition());

	this->target = sf::Vector2f(rand() % 1280, rand() % 768);
}

void ObstacleAvoidance::CreateObstacle(sf::Vector2f _spawnPoint)
{
    Agent* obstacle = new Agent;
    int imageIdx = 7; // wall image
    obstacle->init(this->window, imageIdx, _spawnPoint);
    obstacle->setPosition(_spawnPoint);
	obstacle->setRadius(50.f);
    obstacles.push_back(obstacle);
}

void ObstacleAvoidance::CreateAgent(sf::Vector2f _spawnPoint)
{
	Agent* agent = new Agent;
	int imageIdx = (int)RandomRange(1, 7); // randomly choose image
	agent->init(this->window, imageIdx, _spawnPoint);
	agent->setPosition(_spawnPoint);
	agent->setTarget(this->target);
	agent->setRadius(50.f);
	agents.push_back(agent);
}

void ObstacleAvoidance::update(float dtime, sf::Event* event)
{
	switch (event->type) {
	case sf::Event::MouseButtonReleased:
		if (sf::Event::MouseButtonReleased && event->mouseButton.button == sf::Mouse::Left) // left click to create target
		{
			sf::Vector2f target = sf::Vector2f(
				(float)(sf::Mouse::getPosition(*this->window).x),
				(float)(sf::Mouse::getPosition(*this->window).y)
			);

			if (this->target != target) {
				for (auto& agent : agents)
				{
					agent->setTarget(target);
				}
				this->target = target;
			}

		}

		if (sf::Event::MouseButtonReleased && event->mouseButton.button == sf::Mouse::Right) // Right click to create agent
		{
			sf::Vector2f spawnPoint = sf::Vector2f(
				(float)(sf::Mouse::getPosition(*this->window).x),
				(float)(sf::Mouse::getPosition(*this->window).y)
			);

			if (this->lastClicked != spawnPoint) {
				CreateAgent(spawnPoint);
				this->lastClicked = spawnPoint;
			}
		}

		if (sf::Event::MouseButtonReleased && event->mouseButton.button == sf::Mouse::Middle) // middle click create obstacle
		{
			sf::Vector2f spawnPoint = sf::Vector2f(
				(float)(sf::Mouse::getPosition(*this->window).x),
				(float)(sf::Mouse::getPosition(*this->window).y)
			);

			if (this->lastClicked != spawnPoint) {
				CreateObstacle(spawnPoint);
				this->lastClicked = spawnPoint;
			}

		}
		break;
	default:
		break;
	}

	for (auto& agent : agents) // if no obstacles
	{
		agent->BorderRule();

		sf::Vector2f steering_force = SteeringBehaviour(agent);

		agent->update(steering_force, dtime);
	}
}

void ObstacleAvoidance::render()
{
	this->bg->render(); // bg

	for (auto& obstacle : obstacles)
	{
		obstacle->render();
	}

	for (auto& agent : agents)
	{
		agent->render();
	}

	this->overlay->render(); // overlay
}

const char* ObstacleAvoidance::getTitle() { return "Behaviour: Obstacle Avoidance - Seek"; }


// Steering Behaviours

sf::Vector2f ObstacleAvoidance::SteeringBehaviour(Agent* agent)
{
	float v = Magnitude(agent->getVelocity()) / agent->getMaxVelocity();
	sf::Vector2f ahead = *agent->getPosition() + agent->getVelocity() * v;
	sf::Vector2f halfahead = *agent->getPosition() + agent->getVelocity() * 0.5f * v;

	std::vector <sf::Vector2f> distances, subdistances, avoidForce;

	for (int i = 0; i < obstacles.size(); i++) {

		sf::Vector2f d = ahead - *obstacles[i]->getPosition();
		distances.push_back(d);

		sf::Vector2f subD = halfahead - *obstacles[i]->getPosition();
		subdistances.push_back(subD);

		sf::Vector2f af;
		af = ahead - *obstacles[i]->getPosition();
		af = Normalize(af) * MAX_AVOID_FORCE;

		avoidForce.push_back(af);
	}

	for (int i = 0; i < obstacles.size(); i++) {

		//float radius = RadiusOfCircumCircle(obstacles[i]->getSize().x, obstacles[i]->getSize().y);
		float radius = RadiusOfCircumCircle(agent->getSize().x, agent->getSize().y); // this works better
		float r = radius * 0.5f;

		for (auto* agent : agents) {
			// check that agent wont constantly back into the walls to get away from the target
			if (agent->BorderRule()) {
				*agent->getTarget() = *agent->getPosition();
			}
		}

		// if near an obstacle

		if (Magnitude(distances[i]) <= radius * r || 
			Magnitude(subdistances[i]) <= radius* r || 
			Distance(*agent->getPosition(), *obstacles[i]->getPosition()) <= radius * r) 
		{

			sf::Vector2f desiredV = *agent->getPosition() - *agent->getTarget();
			desiredV = Normalize(desiredV);
			desiredV *= agent->getMaxVelocity();

			sf::Vector2f steeringForce = desiredV - agent->getVelocity();
			steeringForce /= agent->getMaxVelocity();
			steeringForce *= agent->getMaxForce();

			distances.clear();
			subdistances.clear();

			return -steeringForce + avoidForce[i];
		}
		else // not near an obstacle
		{
			sf::Vector2f desiredVelocity = *agent->getPosition() - *agent->getTarget();
			desiredVelocity = Normalize(desiredVelocity);
			desiredVelocity *= agent->getMaxVelocity();

			sf::Vector2f steeringForce = desiredVelocity - agent->getVelocity();
			steeringForce /= agent->getMaxVelocity();
			steeringForce *= agent->getMaxForce();

			return -steeringForce;
		}
	}

	if (obstacles.size() < 1) // no obstacles
	{
		sf::Vector2f desiredVelocity = *agent->getPosition() - *agent->getTarget();
		desiredVelocity = Normalize(desiredVelocity);
		desiredVelocity *= agent->getMaxVelocity();

		sf::Vector2f steeringForce = desiredVelocity - agent->getVelocity();
		steeringForce /= agent->getMaxVelocity();
		steeringForce *= agent->getMaxForce();

		return -steeringForce;
	}

}




// other functions

bool ObstacleAvoidance::lineIntersectsAgent(sf::Vector2f ahead, sf::Vector2f ahead2, Agent* obstacle)
{
	std::cout << "radius: x: " << RadiusOfCircumCircle(obstacle->getLocalSize().x, obstacle->getLocalSize().y) << std::endl;
	return Distance(*obstacle->getCenter(), ahead) <= RadiusOfCircumCircle(obstacle->getLocalSize().x, obstacle->getLocalSize().y) || Distance(*obstacle->getCenter(), ahead2) <= RadiusOfCircumCircle(obstacle->getLocalSize().x, obstacle->getLocalSize().y);
}

float ObstacleAvoidance::Distance(sf::Vector2f a, sf::Vector2f b)
{ 
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)); 
}

float ObstacleAvoidance::RadiusOfCircumCircle(float w, float h)
{
	// the sides cannot be negative
	if (w < 0 || h < 0)
		return -1;

	// Radius of the circumcircle
	float radius = sqrt(pow(w, 2) + pow(h, 2)) / 2;

	// Return the radius
	return radius;
}

sf::Vector2f ObstacleAvoidance::Truncate(sf::Vector2f v, float max)
{
	sf::Vector2f truncated = v;
	if (Magnitude(v) > max)
	{
		truncated = Normalize(truncated) * max;
	}

	return truncated;
}

float ObstacleAvoidance::OppositeFloat(float f) {
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

float ObstacleAvoidance::Magnitude(sf::Vector2f vector)
{
	return (float)sqrt(vector.x * vector.x + vector.y * vector.y);
}

sf::Vector2f ObstacleAvoidance::Normalize(sf::Vector2f vector)
{
	float mag = Magnitude(vector);
	if (mag > 0.0f) {
		return vector / Magnitude(vector);
	}
	else {
		return sf::Vector2f();
	}
}

float ObstacleAvoidance::RandomRange(int min, int max)
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
