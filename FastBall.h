#pragma once
#include "Powerup.h"

#define MOTION_SAMPLES 10

class FastBall : public Powerup
{
public:
	FastBall(const Vector3d &theInitialPosition);
	void reset();
	~FastBall(void);
	void drawMe();
	void updateState();
	void setOwner(Player* player);
	bool mustDestroy();

private:

	GLUquadricObj* glu_icon;
	GLfloat icon_rot;
	GLuint displayListIcon;
	void initDisplayList();
	double elapsed_time;
	Vector3d prev_positions[MOTION_SAMPLES];

};
