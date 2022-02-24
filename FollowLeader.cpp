#include "FollowLeader.h"

FollowLeader::FollowLeader()
{

	this->window = nullptr;
}

FollowLeader::~FollowLeader()
{
	delete bg;
	delete overlay;

	for (auto& agent : agents)
	{
		delete agent;
	}
}

void FollowLeader::init(sf::RenderWindow* _window)
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

void FollowLeader::CreateAgent(sf::Vector2f _spawnPoint)
{
	Agent* agent = new Agent;
	int imageIdx = (int)RandomRange(1, 7); // randomly choose image
	agent->init(this->window, imageIdx, _spawnPoint);
	agent->setPosition(_spawnPoint);
	agent->setTarget(sf::Vector2f(640, 360));
	agent->setRadius(50.f);
	agents.push_back(agent);
}

void FollowLeader::update(float dtime, sf::Event* event)
{
	switch (event->type) {
	case sf::Event::MouseButtonReleased:
		if (sf::Event::MouseButtonReleased && event->mouseButton.button == sf::Mouse::Right) // left click create agent
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

	if (!agents.empty())
	{
		std::vector<int> textures;
		std::vector<Agent*> leaders;
		for (auto& agent : agents)
		{
			if (std::find(textures.begin(), textures.end(), agent->getImageIdx()) == textures.end()) {
				textures.push_back(agent->getImageIdx());
				leaders.push_back(agent); // first agent with n texture becomes leader for that group
			}
		}

		for (auto& agent : agents)
		{
			Agent* leader = nullptr;

			for (auto& _leader : leaders) // select leader of same type
			{
				if (agent == _leader) {
					leader = nullptr;
				}
				else if (agent->getImageIdx() == _leader->getImageIdx()) {
					leader = _leader;
				}
			}

			if (leader == nullptr) 
			{
				agent->BorderRule();

				sf::Vector2f dist = *agent->getTarget() - *agent->getPosition();

				this->angle = float(rand() % 360);

				this->centre = *agent->getPosition() + agent->Normalize(agent->getVelocity()) * this->wanderOffset;

				sf::Vector2f target;
				target.x = this->centre.x + this->wanderRadius * sin(this->angle);
				target.y = this->centre.y + this->wanderRadius * cos(this->angle);

				agent->setTarget(target);

				sf::Vector2f steering_force = Wander(agent);

				agent->update(steering_force, dtime);
			} 
			else
			{
				agent->BorderRule();
				sf::Vector2f steering_force = SteeringBehaviour(agent, leader);
				agent->update(steering_force, dtime);
			}
		}
	}
}

void FollowLeader::render()
{
	this->bg->render(); // bg

	for (auto& agent : agents)
	{
		agent->render();
	}	
	
	this->overlay->render(); // overlay
}

const char* FollowLeader::getTitle() { return "Behaviour: Follow the Leader - Wander"; }


// Steering Behaviours

sf::Vector2f FollowLeader::SteeringBehaviour(Agent* agent, Agent* leader)
{
	sf::Vector2f desiredVelocity = *leader->getPosition() - *agent->getPosition();

	// Calculate the ahead point
	sf::Vector2f tv = Normalize(leader->getVelocity());
	float LEADER_BEHIND_DIST = 0.1f;
	tv *= LEADER_BEHIND_DIST;
	sf::Vector2f ahead = *leader->getPosition() + tv;

	// Calculate the behind point
	tv.x *= -1;
	tv.y *= -1;
	sf::Vector2f behind = *leader->getPosition() + tv;

	float LEADER_SIGHT_RADIUS = 10.f;
	// If the character is on the leader's sight, add a force
	// to evade the route immediately.
	if (isOnLeaderSight(agent, leader, ahead, LEADER_SIGHT_RADIUS)) {
		desiredVelocity += Evade(agent, leader);
	} 

	float slowingRadius = 5.f;
	sf::Vector2f arrival = Arrive(leader, slowingRadius);

	float radius = RadiusOfCircumCircle(leader->getSize().x, leader->getSize().y);
	float desiredSeparation = radius * 2.f;
	sf::Vector2f separation = Separate(leader, desiredSeparation);

	float arrivalWeight = 1.f;
	float separationWeight = 1.f;

	desiredVelocity = desiredVelocity + (arrival * arrivalWeight) + (separation * separationWeight);

	desiredVelocity = Normalize(desiredVelocity);
	desiredVelocity *= leader->getMaxVelocity();

	sf::Vector2f steeringForce = desiredVelocity - leader->getVelocity();
	steeringForce /= leader->getMaxVelocity();
	steeringForce *= leader->getMaxForce();

	return steeringForce;
}

bool FollowLeader::isOnLeaderSight(Agent* agent, Agent* leader, sf::Vector2f leaderAhead, float LEADER_SIGHT_RADIUS)
{
	return Distance(leaderAhead, *agent->getPosition()) <= LEADER_SIGHT_RADIUS || Distance(*leader->getPosition(), *agent->getPosition()) <= LEADER_SIGHT_RADIUS;
}

sf::Vector2f FollowLeader::Wander(Agent* agent)
{
	sf::Vector2f desiredVelocity = *agent->getTarget() - *agent->getPosition(); // move towards target
	desiredVelocity = Normalize(desiredVelocity);
	desiredVelocity *= agent->getMaxVelocity();

	sf::Vector2f steeringForce = desiredVelocity - agent->getVelocity();
	steeringForce /= agent->getMaxVelocity();
	steeringForce *= agent->getMaxForce();

	return steeringForce;
}

sf::Vector2f FollowLeader::Separate(Agent* agent, float desiredSeparation)
{
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

	if (count > 0) // if the agent has neighbours
	{
		desiredVelocity /= (float)count;
	}
	else // if agent has no neighbours
	{
		desiredVelocity = *agent->getTarget() - *agent->getPosition();
	}

	return desiredVelocity;
}

sf::Vector2f FollowLeader::Arrive(Agent* agent, float slowingRadius)
{
	sf::Vector2f desiredV = *agent->getTarget() - *agent->getPosition();
	sf::Vector2f steeringForce = desiredV - agent->getVelocity();
	sf::Vector2f dist = *agent->getTarget() - *agent->getPosition();

	if (Magnitude(dist) < slowingRadius) {
		desiredV *= 2 * Magnitude(desiredV);
		desiredV *= Magnitude(dist) / slowingRadius;
	}

	return desiredV;
}

sf::Vector2f FollowLeader::Evade(Agent* prey, Agent* predator)
{
	sf::Vector2f distance = *predator->getPosition() - *prey->getPosition(); // distance between prey and agent
	float updatesAhead = Magnitude(distance) / prey->getMaxVelocity(); // how many updates the target needs to move from its current position to the pursuer position
	sf::Vector2f futurePosition = *predator->getPosition() + predator->getVelocity() * updatesAhead;

	prey->setTarget(futurePosition);

	return Flee(prey);
}

sf::Vector2f FollowLeader::Flee(Agent* agent)
{
	sf::Vector2f tPos = *agent->getTarget();
	sf::Vector2f aPos = *agent->getPosition();
	sf::Vector2f size = agent->getSize();
	sf::Vector2f v = agent->getVelocity();

	// check that agent wont constantly back into the walls to get away from the target
	if (agent->BorderRule()) {
		*agent->getTarget() = *agent->getPosition();
	}

	sf::Vector2f desiredVelocity = aPos - tPos; // move away from target
	desiredVelocity = Normalize(desiredVelocity);
	desiredVelocity *= agent->getMaxVelocity();

	sf::Vector2f steeringForce = desiredVelocity - v;
	steeringForce /= agent->getMaxVelocity();
	steeringForce *= agent->getMaxForce();

	return steeringForce;
}


// other functions

bool FollowLeader::lineIntersectsAgent(sf::Vector2f ahead, sf::Vector2f ahead2, Agent* obstacle)
{
	std::cout << "radius: x: " << RadiusOfCircumCircle(obstacle->getLocalSize().x, obstacle->getLocalSize().y) << std::endl;
	return Distance(*obstacle->getCenter(), ahead) <= RadiusOfCircumCircle(obstacle->getLocalSize().x, obstacle->getLocalSize().y) || Distance(*obstacle->getCenter(), ahead2) <= RadiusOfCircumCircle(obstacle->getLocalSize().x, obstacle->getLocalSize().y);
}

float FollowLeader::Distance(sf::Vector2f a, sf::Vector2f b)
{
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

float FollowLeader::RadiusOfCircumCircle(float w, float h)
{
	// the sides cannot be negative
	if (w < 0 || h < 0)
		return -1;

	// Radius of the circumcircle
	float radius = sqrt(pow(w, 2) + pow(h, 2)) / 2;

	// Return the radius
	return radius;
}

sf::Vector2f FollowLeader::Truncate(sf::Vector2f v, float max)
{
	sf::Vector2f truncated = v;
	if (Magnitude(v) > max)
	{
		truncated = Normalize(truncated) * max;
	}

	return truncated;
}

float FollowLeader::OppositeFloat(float f) {
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

float FollowLeader::Magnitude(sf::Vector2f vector)
{
	return (float)sqrt(vector.x * vector.x + vector.y * vector.y);
}

sf::Vector2f FollowLeader::Normalize(sf::Vector2f vector)
{
	float mag = Magnitude(vector);
	if (mag > 0.0f) {
		return vector / Magnitude(vector);
	}
	else {
		return sf::Vector2f();
	}
}

float FollowLeader::RandomRange(int min, int max)
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
