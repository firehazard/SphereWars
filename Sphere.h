#ifndef SPHERE_H
#define SPHERE_H

#include "Appearance.h"
#include "sdlheaders.h"
#include "Shape.h"
#include "Vector3d.h"
#include "external/cwmtx/stat_quatern.h"
using namespace CwMtx;

class Sphere: public Shape 
{
public:
	Sphere();
	~Sphere();
	virtual void drawMe(bool insideView);
	virtual void drawShadowVolume();
	virtual void updateState();
	double getRadius();
	virtual void checkCollisionWith(Shape* other_shape);
	void setRadius(double radius);
	void setAppearance(const Appearance &app);
	bool separateSpheres(Shape* other_shape);
	void overrideMass(float mass);
	bool collidesWith(Shape* other_shape);
	void drawAt(Vector3d pos, GLfloat alpha);
	
	void rotateBy(const Vector3d &rotAxis, double rot_amt);

private:
	CWTSQuaternion<double> orient;
	Appearance appearance;
	double radius, init_radius;
	double getCircumference();
	double calcRotationAmt(); // This should probably take a "tic amount"
	Vector3d calcRotationAxis();
	void initDisplayLists();
	void setRadius(double radius, bool isRadiusAlreadySet);
	void drawSphere();
};





#endif
