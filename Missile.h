#ifndef MISSILE_H_
#define MISSILE_H_

class Powerup;

#include "Powerup.h"
#include "Shape.h"
#include "MissileParticleEffects.h"
#include <math/vector3.h>

class MissileParticleEffects;

class Missile: public Powerup 
{
public:
	Missile(const Vector3d &theInitialPosition);
	~Missile(void);
	void reset();
	void setOwner(Player *player);
	void drawMe();
	void drawShadowVolume();
	void updateState();

	void launch(Player* player, Vector3d direction);
	void steer(Player* player,  Vector3d force);

	Vector3d getVelocity();
	
	float getRadius();
	bool mustDestroy();
	
	bool playerLaunched(Player *owner);
	bool hasLaunched();

private:
	Powerup::gluObject glu_missiles[LEVELS_OF_DETAIL];
	GLUquadricObj* glu_closings[LEVELS_OF_DETAIL];

	MissileParticleEffects* particles;
	Vector3d velocity;
	int baseRadius;
	int height;
	void initDisplayLists();
	bool launched;
	bool collided;
	void checkCollisionWith(Shape* other_shape);
	float powerup_angle;
	void createCollisionParticleEffects(Vector location);
	
};



#endif
