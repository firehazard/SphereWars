#include "Shape.h" 
#include "globals.h"
#include <iostream>
using namespace std;

Shape::Shape(void)
 : speed(0.0), mass(1.0), 
   velocity(0,0,0), position(0,0,0),
   contactingStage(false)
{
	
	for (int i=0;i<LEVELS_OF_DETAIL;i++){
		glu_objects[i].glu_shape = gluNewQuadric();
		glu_objects[i].displayListId = 0;
		gluQuadricNormals(glu_objects[i].glu_shape, GL_SMOOTH);
		gluQuadricTexture(glu_objects[i].glu_shape, GL_TRUE);
	}
	lastStageNormal = Vector3d::zero;
}

Shape::~Shape(void)
{
}

Vector3d Shape::getVelocity(){
	return velocity;
}

void Shape::setVelocity(const Vector3d &vel){
	velocity.setX(vel.getX());
	velocity.setY(vel.getY());
	velocity.setZ(vel.getZ());
}




Vector3d Shape::getPosition() const {
	return position;
}

void Shape::setPosition(const Vector3d &pos){
	position.setX(pos.getX());
	position.setY(pos.getY());
	position.setZ(pos.getZ());
}

void Shape::setOldPosition(Vector3d old_pos)
{
	old_position = old_pos;
}


Vector3d Shape::getOldPosition()
{
	return old_position;
}



void Shape::drawMe(bool insideView)
{
}

void Shape::drawShadowVolume() {
}


void Shape::checkCollisionWith(Shape* other_shape){
}

void Shape::applyForce(const Vector3d &force)
{
	velocity.incX(force.getX());
	velocity.incY(force.getY());
	velocity.incZ(force.getZ());
}


void Shape::updateState()
{
	position.incX(velocity.getX()*tick_delta);
	position.incY(velocity.getY()*tick_delta);
	position.incZ(velocity.getZ()*tick_delta);
}

double Shape::getMass()
{
	return mass;
}
bool Shape::isContactingStage() const
{ 
	return contactingStage;
}

void Shape::setLastStageNormal(const Vector3d &stageNormal) 
{
	lastStageNormal = stageNormal;
}

// Takes the given force, gets its
// projection in the plane we're on, scales the 
// projection to be as big as the original force vector,
// and applies the resulting force. 
void Shape::applyForceAdjustingForStage(const Vector3d &force)
{
  if (!force.isZero()) {
	  if (lastStageNormal.isZero()) {
			// We don't have a stored normal => just fall back on applying force directly.
			applyForce(force);
		} else {
			Vector3d tmp = force.crossProduct(lastStageNormal);
			Vector3d projection = lastStageNormal.crossProduct(tmp);
			// Now we have a vector in the plane, projected from the force
			
			Vector3d finalForce = projection.getCopyWithMagnitude(force.getMagnitude());
			applyForce(finalForce);
		}
  }
}

void Shape::applyFriction()
{
	double surface_friction = theWorld->getStage()->getFrictionAt(position);
	velocity.decX(surface_friction * velocity.getX());
	velocity.decY(surface_friction * velocity.getY());
	velocity.decZ(surface_friction * velocity.getZ());
}

void Shape::setContactingStage(bool newVal)
 { 
 	contactingStage = newVal;
 }

