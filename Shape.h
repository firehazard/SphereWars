#ifndef SHAPE_H
#define SHAPE_H

#include "sdlheaders.h"
#include "Vector3d.h"
#include "Constants.h"


class Shape
{

protected:
	double speed;
	double mass;
	Vector3d velocity;
	Vector3d position;
	Vector3d lastStageNormal;
	bool contactingStage;	

	struct gluObject {
		GLUquadricObj* glu_shape;		
		GLuint displayListId;
	};
	
	gluObject glu_objects[LEVELS_OF_DETAIL];
	
	Vector3d old_position; //pre-collision position
	

public:
	Shape();


	virtual ~Shape();
	void applyForce(const Vector3d &force);
	void applyForceAdjustingForStage(const Vector3d &force);
	Vector3d getVelocity();
	void setVelocity(const Vector3d &vel);
	Vector3d getPosition() const;
	void setPosition(const Vector3d &pos);
	double getMass();
	bool isContactingStage() const;
	void setLastStageNormal(const Vector3d &stageNormal);
	
	virtual void drawMe(bool insideView);
	virtual void drawShadowVolume();

	virtual void updateState();
	virtual void checkCollisionWith(Shape* other_shape);

	// TODO: I'm not sure why we made this virtual -- is there a reason?
	// (It already has a definition in Shape.cpp)
	virtual void applyFriction();

	void setContactingStage(bool newVal);
	Vector3d getOldPosition();
	void setOldPosition(Vector3d old_pos);


};


#endif

