#include "FollowLeader_Seek.h"

FollowLeader_Seek::FollowLeader_Seek()
{
	this->window = nullptr;
}

FollowLeader_Seek::~FollowLeader_Seek()
{
	delete bg;
	delete overlay;

	for (auto& agent : agents)
	{
		delete agent;
	}
}

void FollowLeader_Seek::init(sf::RenderWindow* _window)
{
	this->window = _window;

	this->bg = new Agent;
	bg->init(this->window, 0, *bg->getPosition());

	this->overlay = new Agent;
	overlay->init(this->window, 9, *overlay->getPosition());
}

void FollowLeader_Seek::CreateAgent(sf::Vector2f _spawnPoint)
{
	Agent* agent = new Agent;
	int imageIdx = (int)RandomRange(1, 7); // randomly choose image
	agent->init(this->window, imageIdx, _spawnPoint);
	agent->setPosition(_spawnPoint);
	agent->setTarget(sf::Vector2f(640, 360));
	agent->setRadius(50.f);
	agents.push_back(agent);
}

void FollowLeader_Seek::update(float dtime, sf::Event* event)
{
	switch (event->type) {
	case sf::Event::MouseButtonReleased:
		if (sf::Event::MouseButtonReleased && event->mouseButton.button == sf::Mouse::Left) // set target
		{
			sf::Vector2f target = sf::Vector2f(
				(float)(sf::Mouse::getPosition(*this->window).x),
				(float)(sf::Mouse::getPosition(*this->window).y)
			);

			if (this->lastClickPosition != target) 
			{
				for (auto* agent : agents) 
				{
					agent->setTarget(target);
				}
				std::cout << "Mouse Clicked at: x:" << target.x << ", y: " << target.y << std::endl;
			}

		}
		if (sf::Event::MouseButtonReleased && event->mouseButton.button == sf::Mouse::Right) // right click create agent
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

			if (leader == nullptr) // if first item of type to spawn
			{
				agent->BorderRule();

				sf::Vector2f steering_force = Seek(agent);

				agent->update(-steering_force, dtime);
			}
			else
			{
				agent->BorderRule();
				sf::Vector2f steering_force = SteeringBehaviour(agent, leader);
				agent->update(-steering_force, dtime);
			}
		}
	}
}

void FollowLeader_Seek::render()
{
	this->bg->render(); // bg

	for (auto& agent : agents)
	{
		agent->render();
	}

	this->overlay->render(); // overlay
}

const char* FollowLeader_Seek::getTitle() { return "Behaviour: Follow the Leader - Seek"; }

sf::Vector2f FollowLeader_Seek::SteeringBehaviour(Agent* agent, Agent* leader)
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

	float arrivalWeight = 10.f;
	float separationWeight = 100.f;

	desiredVelocity = desiredVelocity + (arrival * arrivalWeight) + (separation * separationWeight);

	desiredVelocity = Normalize(desiredVelocity);
	desiredVelocity *= leader->getMaxVelocity();

	sf::Vector2f steeringForce = desiredVelocity - leader->getVelocity();
	steeringForce /= leader->getMaxVelocity();
	steeringForce *= leader->getMaxForce();

	return steeringForce;
}

bool FollowLeader_Seek::isOnLeaderSight(Agent* agent, Agent* leader, sf::Vector2f leaderAhead, float LEADER_SIGHT_RADIUS)
{
	return Distance(leaderAhead, *agent->getPosition()) <= LEADER_SIGHT_RADIUS || Distance(*leader->getPosition(), *agent->getPosition()) <= LEADER_SIGHT_RADIUS;
}

sf::Vector2f FollowLeader_Seek::Separate(Agent* agent, float desiredSeparation)
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

sf::Vector2f FollowLeader_Seek::Arrive(Agent* agent, float slowingRadius)
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

sf::Vector2f FollowLeader_Seek::Evade(Agent* prey, Agent* predator)
{
	sf::Vector2f distance = *predator->getPosition() - *prey->getPosition(); // distance between prey and agent
	float updatesAhead = Magnitude(distance) / prey->getMaxVelocity(); // how many updates the target needs to move from its current position to the pursuer position
	sf::Vector2f futurePosition = *predator->getPosition() + predator->getVelocity() * updatesAhead;

	prey->setTarget(futurePosition);

	return Flee(prey);
}

sf::Vector2f FollowLeader_Seek::Flee(Agent* agent)
{
	// check that agent wont constantly back into the walls to get away from the target
	if (agent->BorderRule()) {
		*agent->getTarget() = *agent->getPosition();
	}

	sf::Vector2f desiredVelocity = *agent->getPosition() - *agent->getTarget(); // move away from target
	desiredVelocity = Normalize(desiredVelocity);
	desiredVelocity *= agent->getMaxVelocity();

	sf::Vector2f steeringForce = desiredVelocity - agent->getVelocity();
	steeringForce /= agent->getMaxVelocity();
	steeringForce *= agent->getMaxForce();

	return steeringForce;
}

sf::Vector2f FollowLeader_Seek::Seek(Agent* agent)
{
	sf::Vector2f desiredVelocity = *agent->getPosition() - *agent->getTarget();
	desiredVelocity = Normalize(desiredVelocity);
	desiredVelocity *= agent->getMaxVelocity();

	sf::Vector2f steeringForce = desiredVelocity - agent->getVelocity();
	steeringForce /= agent->getMaxVelocity();
	steeringForce *= agent->getMaxForce();

	return steeringForce;
}

bool FollowLeader_Seek::lineIntersectsAgent(sf::Vector2f ahead, sf::Vector2f ahead2, Agent* obstacle)
{
	std::cout << "radius: x: " << RadiusOfCircumCircle(obstacle->getLocalSize().x, obstacle->getLocalSize().y) << std::endl;
	return Distance(*obstacle->getCenter(), ahead) <= RadiusOfCircumCircle(obstacle->getLocalSize().x, obstacle->getLocalSize().y) || Distance(*obstacle->getCenter(), ahead2) <= RadiusOfCircumCircle(obstacle->getLocalSize().x, obstacle->getLocalSize().y);
}

float FollowLeader_Seek::Distance(sf::Vector2f a, sf::Vector2f b)
{
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

float FollowLeader_Seek::RadiusOfCircumCircle(float w, float h)
{
	// the sides cannot be negative
	if (w < 0 || h < 0)
		return -1;

	// Radius of the circumcircle
	float radius = sqrt(pow(w, 2) + pow(h, 2)) / 2;

	// Return the radius
	return radius;
}

float FollowLeader_Seek::Magnitude(sf::Vector2f vector)
{
	return (float)sqrt(vector.x * vector.x + vector.y * vector.y);
}

sf::Vector2f FollowLeader_Seek::Normalize(sf::Vector2f vector)
{
	float mag = Magnitude(vector);
	if (mag > 0.0f) {
		return vector / Magnitude(vector);
	}
	else {
		return sf::Vector2f();
	}
}

float FollowLeader_Seek::RandomRange(int min, int max)
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

