#include "StageMind.h"
#include "globals.h"
#include "Constants.h"
#include "SDLEngine.h"
#include "Utils.h"
#include <math/matrix4.h>

#include <iostream>


using namespace std;



#define AUTOSPIN_ANGLE 20.0f
#define AUTOSPIN_SPEED 100.0f
#define PRE_SPIN_SPEED 50.0f
#define AUTOSPIN_TIME 0.5
#define TIME_BETWEEN_AUTOSPINS 4

StageMind::StageMind(void)
{
	elapsed_time = -1;
	spin_enabled = false;
	curr_ar_angle = 0;
	pre_ar_angle = 0;
	pre_spin_done = false;
	elapsed_time = 0;

}

StageMind::~StageMind(void)
{
}


void StageMind::updateState()
{
	if (!theWorld->getAutoSpinEnabled() && !spin_enabled) return;

	elapsed_time+=tick_delta;

	if (elapsed_time > TIME_BETWEEN_AUTOSPINS){
		spin_enabled = true;
		if (!pre_spin_done){
			beginAutoSpin();
		}
		else {
			autoSpin();
		}
	}

	if (elapsed_time > TIME_BETWEEN_AUTOSPINS + AUTOSPIN_TIME) {
		spin_enabled = false;
		elapsed_time = 0;
	}

}


void StageMind::beginAutoSpin()
{
	pre_ar_angle+=(tick_delta*PRE_SPIN_SPEED );
	if (pre_ar_angle>AUTOSPIN_ANGLE){
		pre_ar_angle = AUTOSPIN_ANGLE;
		pre_spin_done = true;
	}	
	theWorld->getStage()->rotateBy(pre_ar_angle, Vector3d::xAxis);
}


void StageMind::autoSpin()
{
	ar_axis = Vector3d::xAxis;
	ar_axis.rotateBy(curr_ar_angle, Vector3d(0,1,0));
	theWorld->getStage()->rotateBy(AUTOSPIN_ANGLE,ar_axis);
	curr_ar_angle+=(tick_delta*AUTOSPIN_SPEED);
	if (curr_ar_angle>=DEGREES_IN_A_CIRCLE) {
		curr_ar_angle-=DEGREES_IN_A_CIRCLE;
	}
}
