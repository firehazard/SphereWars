#pragma once
#include "Powerup.h"

class BigBall :	public Powerup
{
public:
	BigBall(const Vector3d &theInitialPosition);
	~BigBall(void);
	void reset();
	void drawMe();
	void updateState();
	void setOwner(Player* player);
	bool mustDestroy();
	


private:
	GLUquadricObj* glu_icon;
	GLuint displayListIcon;
	void initDisplayList();
	double elapsed_time;
	bool done_shrinking;
	float init_radius;
	bool first_shrink;
	float scale_factor;
	bool is_add_factor;

};
