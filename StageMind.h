#pragma once
#include "sdlheaders.h"
#include "Vector3d.h"

class StageMind
{
public:
	StageMind(void);
	~StageMind(void);
	void updateState();
	
private:
	double elapsed_time;
	void autoSpin();
	bool spin_enabled;

	//ar = auto_rotate
	Vector3d ar_axis;
	float curr_ar_angle;

	float pre_ar_angle;

	void beginAutoSpin();
	bool pre_spin_done;


};
