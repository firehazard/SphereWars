#ifndef PLAYER_H
#define PLAYER_H

class Missile;
#include "Missile.h"
#include "Shape.h"
#include "Appearance.h"
#include <vector>


class Player
{
public:
	Player(int player_num);
	~Player();
	void drawShadowVolume();
	void setShape(int shape_type);
	void setAppearance(const Appearance &app);
	void setSpawnPoint(const Vector3d &theSpawnPoint);	
	void updateState();
	Shape* getShape();
	int getNumLives() const;
	void resetDeath();
	bool getDeath();
	bool hasFastPowerup();
	void setFastPowerup(bool val);
	void toggleTransparent();
	bool getTransparent();
	void incMissileCount();
	void decMissileCount();
	void clearMissileCount();
	int getMissileCount(); // for HUD

private:
	Shape* my_shape;
	int player_num;
	int num_lives;
	bool dead;
	bool fast_powerup;
	bool isTransparent;
	int missile_count;
	Vector3d spawnPoint;
};


#endif
