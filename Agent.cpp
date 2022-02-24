#include "Agent.h"

Agent::Agent() :  velocity(sf::Vector2f(0.f, 0.f)),
				  mass(0.01f),
				  max_force(50),
				  max_velocity(300),
				  orientation(0)
{
	agentSprite;
	agentTexture;
	imageIdx = 3;
	window = nullptr;
}


Agent::~Agent() { }


void Agent::init(sf::RenderWindow* window, int textureIdx, sf::Vector2f position)
{

	setWindow(window);
	setTexture(textureIdx);

	if (textureIdx > 0)
	{
		resetTargetArea();
		setTargetToCenter();

		float x = (float)(this->window->getSize().x / 2);
		float y = (float)(this->window->getSize().y / 2);
		setPosition(sf::Vector2f(x, y));
		this->agentSprite.setOrigin(agentSprite.getLocalBounds().width/2,agentSprite.getLocalBounds().height/2);
	}
}

void Agent::update(sf::Vector2f steering_force, float dtime)
{
	if (!stopSpinning) {


		sf::Vector2f acceleration = steering_force / mass;
		sf::Vector2f newVelocity = getVelocity() + acceleration * dtime;
		setVelocity(newVelocity);

		setVelocity(
			Truncate(
				getVelocity(),
				getMaxVelocity()
			)
		);

		setPosition(*getPosition() + getVelocity() * dtime);

		// Update orientation
		setOrientation(
			RadianToDegree(
				atan2(
					getVelocity().y,
					getVelocity().x
				)
			)
		);
	}
}

void Agent::render() 
{ 
	this->window->draw(this->agentSprite); 
}







bool Agent::BorderRule()
{
	bool borderCrossed = false;

	sf::Vector2f targetPos = *getPosition();
	sf::Vector2f currentPos = targetPos;
	float halfX = getSize().x / 2;
	float halfY = getSize().y / 2;

	float x = this->window->getPosition().x;
	float y = this->window->getPosition().y;
	float w = this->window->getSize().x;
	float h = this->window->getSize().y;

	
	if (currentPos.x + halfX < 0) 
	{
		targetPos.x = targetPos.x + w;
		targetPos.y = targetPos.y - h;
		borderCrossed = true;
	}

	if (currentPos.y + halfY < 0)
	{
		targetPos.y = targetPos.y + y;
		targetPos.x = targetPos.x + w;
		borderCrossed = true;
	}

	if (currentPos.y - halfY > h)
	{
		targetPos.y = targetPos.y - h;
		targetPos.x = targetPos.x + w;
		borderCrossed = true;
	} 

	if (currentPos.x - halfX > w)
	{
		targetPos.x = targetPos.x - w;
		targetPos.y = targetPos.y + y;
		borderCrossed = true;
	}

	if (targetPos.x < 0) {
		targetPos.x = targetPos.x + w;
		borderCrossed = true;
	}

	if (targetPos.y < 0) {
		targetPos.y = targetPos.y + h;
		borderCrossed = true;
	}	
	
	if (targetPos.x > w) {
		targetPos.x = targetPos.x - w;
		borderCrossed = true;
	}	
	
	if (targetPos.y > h) {
		targetPos.y = targetPos.y - h;
		borderCrossed = true;
	}

	if (currentPos != targetPos) {
		setPosition(sf::Vector2f(targetPos));
		borderCrossed = true;
	}
	
	return borderCrossed;
}





void Agent::setPosition(sf::Vector2f _position) 
{ 
	this->agentSprite.setPosition(_position);
	this->position = this->agentSprite.getPosition();
}

void Agent::setWindow(sf::RenderWindow* _window) 
{ 
	this->window = _window; 
}

void Agent::setTargetArea(float _x, float _y, float _w, float _h)
{
	this->targetArea = sf::FloatRect(_x, _y, _w, _h);
}

void Agent::setTargetAreaByMargin(float _margin)
{
	float x = targetArea.left + _margin;
	float y = targetArea.top + _margin;
	float w = targetArea.width - _margin;
	float h = targetArea.height - _margin;

	setTargetArea(x, y, w, h);
}

void Agent::setTexture(int _imageIdx)
{

	std::string filenames[10] = { "bg.png", "cat.png", "croc.png" , "frog.png" , "oni.png" , "ogre.png" , "skull.png" , "prince.png", "wall.png", "overlay.png"};
	std::string filename = filenames[_imageIdx];
	this->imageIdx = _imageIdx;

	if (!this->agentTexture.loadFromFile(filename)) 
	{ 
		std::cout << "ERROR: Failed to load image file: " << filename << "\n"; 
	}

	agentTexture.setSmooth(true);

	this->agentSprite.setTexture(this->agentTexture);
	

	if (_imageIdx == 0 || _imageIdx == 9) // bg or overlay
	{
		agentTexture.setRepeated(true);
		agentSprite.setTextureRect(sf::IntRect(0.f, 0.f, this->window->getSize().x, this->window->getSize().y));

	}
	else if (_imageIdx == 8) // wall
	{
		this->agentSprite.setTexture(this->agentTexture);

		// scale
		agentSprite.setScale(sf::Vector2f(0.5f, 0.5f)); // absolute scale factor
		agentSprite.scale(sf::Vector2f(0.1f, 0.1f)); // factor relative to the current scale
	}
	else
	{
		this->agentSprite.setTexture(this->agentTexture);

		// scale
		agentSprite.setScale(sf::Vector2f(0.5f, 0.5f)); // absolute scale factor
		agentSprite.scale(sf::Vector2f(0.2f, 0.2f)); // factor relative to the current scale
	}
}

sf::Vector2f Agent::getSize()
{
	return sf::Vector2f(agentSprite.getGlobalBounds().width, agentSprite.getGlobalBounds().height);
}

sf::Vector2f Agent::getLocalSize()
{
	return sf::Vector2f(agentSprite.getLocalBounds().width, agentSprite.getLocalBounds().height);
}

void Agent::resetTargetArea()
{

	float extraMargin = 0.f;

	// make sure entire agent image stays in the window
	float margin_x = getSize().x/3;
	float margin_y = getSize().y/3;

	// TopLeft
	float x = margin_x;
	float y = margin_y;
	float w = this->window->getSize().x - (margin_x*3);
	float h = this->window->getSize().y - (margin_y*3);

	setTargetArea(x, y, w, h);

}

sf::FloatRect Agent::getTargetArea() 
{
	return this->targetArea;
}

sf::FloatRect Agent::fitsInBox(sf::FloatRect* _smallBox, sf::FloatRect* _bigBox) 
{
	if (_smallBox->left > _smallBox->width) // make sure x is smaller than w
	{
		Swap(&_smallBox->left, &_smallBox->width);
	}

	if (_smallBox->top > _smallBox->height) // make sure y is smaller than h
	{ 
		Swap(&_smallBox->top, &_smallBox->height);
	}

	if (OutOfBounds(_smallBox->left, _bigBox->left, _bigBox->width))
	{ 
		_smallBox->left = _bigBox->left;
	}

	if (OutOfBounds(_smallBox->top, _bigBox->top, _bigBox->height))
	{ 
		_smallBox->top = _bigBox->top;
	}

	if (OutOfBounds(_smallBox->width, _bigBox->left, _bigBox->width))
	{ 
		_smallBox->width = _bigBox->width;
	}

	if (OutOfBounds(_smallBox->height, _bigBox->top, _bigBox->height))
	{ 
		_smallBox->height = _bigBox->height;
	}

	return sf::FloatRect(_bigBox->left, _bigBox->top, _smallBox->width, _smallBox->height);
}

void Agent::Swap(float* _x, float* _y) 
{ 
	float* temp = _x;	
	_x = _y; 
	_y = temp;
}

bool Agent::OutOfBounds(float _x, float _a, float _b) 
{ 
	return (_x <= _a && _x >= _b) ? true: false; 
} 





// for behaviour class

sf::Vector2f Agent::getRandomPosition(sf::FloatRect targetArea) {

	int x = static_cast<int>(targetArea.left);
	int w = static_cast<int>(targetArea.width);
	int y = static_cast<int>(targetArea.top);
	int h = static_cast<int>(targetArea.height);

	float xpos = RandomRange(x, w);
	float ypos = RandomRange(y, h);

	sf::Vector2f target = sf::Vector2f(xpos, ypos);

	return target;
}

sf::Vector2f* Agent::getTarget() 
{ 
	return &this->target; 
}

void Agent::setTargetToCenter() 
{
	sf::Vector2f center = sf::Vector2f(targetArea.width/2, targetArea.height/2);

	setTarget(center);
}

void Agent::setTarget(sf::Vector2f _target)
{
	sf::Vector2f t;

	// target can't go left of the screen border
	_target.x <= targetArea.left ? t.x = targetArea.left : t.x = _target.x;

	// target can't go above the screen
	_target.y <= targetArea.top ? t.y = targetArea.top : t.y = _target.y;

	// target can't go right of the screen border
	_target.x >= targetArea.width ? t.x = targetArea.width : t.x = _target.x;

	// target  can't go below the screen
	_target.y >= targetArea.height ? t.y = targetArea.height : t.y = _target.y;

	if (*getPosition() == t)
	{
		t = sf::Vector2f(targetArea.width / 2, targetArea.height /2 );
	}

	this->target = t;
}

float Agent::RandomRange(int min, int max) {

	int n = max - min + 1;
	int remainder = RAND_MAX % n;
	int x;

	do {
		x = rand();
	} while (x >= RAND_MAX - remainder);

	float random = static_cast<float>(min + x % n);
	return random;
}



void Agent::setOrientation(float _orientation)
{
	this->agentSprite.setRotation(_orientation);
	this->orientation = _orientation;
}

float Agent::getOrientation()
{
	return this->orientation;
}

sf::Vector2f Agent::getOrigin() 
{
	return this->agentSprite.getOrigin();
}


// for Behaviour::steeringBehaviour function

sf::Vector2f* Agent::getPosition() 
{
	return &this->position;
}

sf::Vector2f Agent::getVelocity() 
{ 
	return this->velocity; 
}

float& Agent::getMaxVelocity()
{
	return this->max_velocity;
}

float Agent::getMaxForce()
{
	return this->max_force;
}






// for Agent::update function

float Agent::getMass() 
{ 
	return this->mass; 
}

float Agent::RadianToDegree(float radian) 
{ 
	return float(radian * (180 / (float)(2 * acos(0.0)))); 
} //degree = radian * (180/pi)

sf::Vector2f Agent::Truncate(sf::Vector2f v, float max)
{
	sf::Vector2f truncated = v;
	if (Magnitude(v) > max)
	{
		truncated = Normalize(truncated) * max;
	}

	return truncated;
}

void Agent::setVelocity(sf::Vector2f _velocity) 
{ 
	this->velocity = _velocity; 
}

float Agent::Magnitude(sf::Vector2f vector) 
{ 
	return (float)sqrt(vector.x * vector.x + vector.y * vector.y); 
}

sf::Vector2f Agent::Normalize(sf::Vector2f vector) 
{ 
	return Magnitude(vector) > 0.0f ? vector / Magnitude(vector) : sf::Vector2f(); 
}

void Agent::setTargetToRandomEdge() 
{
	int a = RandomRange(0, 4);
	int b = RandomRange(0, 2); 

	if (b == 0)
		b = -1;

	b = a - b; // select point to the left or right of selected point

	std::vector<sf::Vector2f> points = getTargetAreaVertices();

	setTarget(RandPtBt2Pts(points[a], points[a - b]));
}

std::vector<sf::Vector2f> Agent::getTargetAreaVertices()
{
	float left = getTargetArea().left;
	float top = getTargetArea().top;
	float width = getTargetArea().width;
	float height = getTargetArea().height;

	std::vector<sf::Vector2f> points;

	points.push_back(sf::Vector2f(left, top));
	points.push_back(sf::Vector2f(width / 2, top));
	points.push_back(sf::Vector2f(width, top));
	points.push_back(sf::Vector2f(width, height / 2));
	points.push_back(sf::Vector2f(width, height));
	points.push_back(sf::Vector2f(width / 2, height));
	points.push_back(sf::Vector2f(left, height));
	points.push_back(sf::Vector2f(left, height / 2));

	return points;
}

int Agent::OppositeInt(int index, int range) 
{
	int splitPoint = range / 2;

	int oppositeInt = index + splitPoint;

	if (oppositeInt < 0) // if negative
	{
		oppositeInt = abs(oppositeInt);
	}

	return oppositeInt;
}

sf::Vector2f Agent::RandPtBt2Pts(sf::Vector2f a, sf::Vector2f b)
{
	float x = RandomRange(a.x, b.x);
	float y = RandomRange(a.y, b.y);

	return sf::Vector2f(x, y);
}

void Agent::setMaxVelocity(float _max_velocity) { this->max_velocity = _max_velocity; }


void Agent::setMass(float _mass) { this->mass = _mass; }


sf::Sprite* Agent::getSprite() { return &this->agentSprite; }

void Agent::setRadius(float _radius)
{
	this->radius = _radius;
}

float Agent::getRadius()
{
	return this->radius;
}

sf::Vector2f* Agent::getCenter() { 
	sf::Vector2f v = sf::Vector2f(getLocalSize().x / 2, getLocalSize().y / 2);
	return &v; 
}

int Agent::getImageIdx()
{
	return this->imageIdx;
}

static inline float RadiusOfCircumCircle(float w, float h)
{
	// the sides cannot be negative
	if (w < 0 || h < 0)
		return -1;

	// Radius of the circumcircle
	float radius = sqrt(pow(w, 2) + pow(h, 2)) / 2;

	// Return the radius
	return radius;
}
