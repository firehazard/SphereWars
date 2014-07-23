#include "Player.h"
#include "Sphere.h"
#include "Shape.h"
#include "globals.h"
#include "Sound.h"
#include "SDLEngine.h"

#define INIT_LIVES    4
#define DEATH_DISTANCE 500

Player::Player(int player_num)
{
	this->player_num = player_num;
	num_lives = INIT_LIVES;
	dead = false;
	fast_powerup = false;
	isTransparent = false;
	missile_count = 0;
}


Player::~Player(void)
{
}


void Player::setShape(int shapeType)
{
	// TODO: Take shapeType into consideration
	// when we have more shapes
	my_shape = new Sphere();
}

void Player::setAppearance(const Appearance &app)
{	
	((Sphere *)my_shape)->setAppearance(app);
}

void Player::setSpawnPoint(const Vector3d &theSpawnPoint) {
	spawnPoint = theSpawnPoint;
}

void Player::drawShadowVolume() {
	my_shape->drawShadowVolume();
}

void Player::setFastPowerup(bool val)
{
	fast_powerup = val;
}

bool Player::hasFastPowerup()
{
	return fast_powerup;
}

void Player::updateState()
{
	if (num_lives==0) return;
	my_shape->updateState();

	if (my_shape->getPosition().getMagnitude()>DEATH_DISTANCE) {
		dead = true;
		num_lives--;
		cout<<"Player "<<player_num<<" has died. "<<num_lives<<" Lives Left. "<<endl;

		if (num_lives>0) {
			while (true) {
				// TODO DWH: Re-enable this if/when we have getRandomPosition() working well
				// for a generalized stage
				//my_shape->setPosition(theWorld->getStage()->getRandomPosition());
				my_shape->setPosition(spawnPoint);
				if (!theWorld->playerCollides(this)) break;
			}
			my_shape->setVelocity(Vector3d(0,0,0));
		}
	}

}


bool Player::getTransparent()
{
	return isTransparent;
}


void Player::toggleTransparent()
{
	isTransparent = !isTransparent;
}


void Player::incMissileCount()
{
	missile_count++;
}

void Player::decMissileCount()
{
	missile_count--;
}

int Player::getMissileCount()
{
	return missile_count;
}

void Player::clearMissileCount()
{
	missile_count = 0;
}


void Player::resetDeath()
{
	dead = false;
}

bool Player::getDeath()
{
	return dead;
}

Shape* Player::getShape()
{
	return my_shape;
}

int Player::getNumLives() const
{
	return num_lives;
}
