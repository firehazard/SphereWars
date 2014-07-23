#include "Sphere.h"
#include "Utils.h"
#include "SDLEngine.h"
#include "Constants.h"
#include "globals.h"
#include <math.h>
#include <math/point.h>
#include <math/matrix4.h>
#include <vector>
#include <assert.h>

// For debugging output
#include <iostream> 
using namespace std;

#define SHADOW_FUDGE 1.025f

// TODO: Put the quat stuff in a quat_utility class or something
// in another file
#define X_IDX 0
#define Y_IDX 1
#define Z_IDX 2
#define W_IDX 3
double getQuatAngle(const CWTSQuaternion<double> &quat) 
{
	return Utils::radToDeg(2.0 * acos( quat[W_IDX] ));
}

Vector3d getQuatAxis(const CWTSQuaternion<double> &quat) 
{
	Vector3d axis(quat[X_IDX], quat[Y_IDX], quat[Z_IDX]);
	if (!axis.isZero()) {
		axis.normalize();
	}
	return axis;
}

CWTSQuaternion<double> buildQuat(const Vector3d &axis, double angle)
{
	double x,y,z,w;
	if (axis.isZero()) {
		x = y = z = 0;
		w = 1;
	} else {
		Vector3d unit_axis = axis.getUnitVector();
		double angleInRadians = Utils::degToRad(angle);
		double cosVal = cos(angleInRadians / 2); 				
		double sin_val = sin(angleInRadians / 2);
		
		x = unit_axis.getX() * sin_val;
		y = unit_axis.getY() * sin_val;
		z = unit_axis.getZ() * sin_val;
		w = cosVal;				
	}
	return 	CWTSQuaternion<double>(x,y,z,w);
}

Sphere::Sphere()
{
	Shape();
	orient[X_IDX] = 0;
	orient[Y_IDX] = 0;
	orient[Z_IDX] = 0;
	orient[W_IDX] = 1;
	
	setRadius(1, false);
	setRadius(2, true);
	
	//setRadius(2, false);
	//initDisplayList();
	memset(&appearance, 0, sizeof(Appearance));
	appearance.texNum = INVALID_TEXTURE;
}


Sphere::~Sphere()
{
	for (int i=0;i<LEVELS_OF_DETAIL;i++){
		if (glu_objects[i].displayListId != 0) {
			glDeleteLists(glu_objects[i].displayListId, 1);
		}
	}
}

void Sphere::drawAt(Vector3d pos, GLfloat alpha)
{
	glPushMatrix();

	glTranslatef(pos.getX(), pos.getY(), pos.getZ());	

	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);

	glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE);
	glBlendColor(0,0,0,alpha);

	if (init_radius!=radius){
		float scale_amount = radius/init_radius;
		glScalef(scale_amount,scale_amount,scale_amount);
	}

	drawSphere();	

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	glPopMatrix();


}



void Sphere::drawSphere()
{
	int dist = SDLEngine::getInstance()->getDistanceFromCam(position);
	if (dist>MAX_DIST_MDETAIL) dist = MAX_DIST_MDETAIL;
	if (dist<MIN_DIST_FDETAIL) dist = MIN_DIST_FDETAIL;
	int detail_level = 0; //0 means most details
	float denom = MAX_DIST_MDETAIL - MIN_DIST_FDETAIL;
	float num  = dist - MIN_DIST_FDETAIL;
	detail_level = (int)((float)(num/denom)* LEVELS_OF_DETAIL);
	float rad_diff = radius - init_radius - 1;
	if (rad_diff<0) rad_diff = 0;
	int extra_detail = (int)(rad_diff*((float)LEVELS_OF_DETAIL/10.0));
	detail_level-=extra_detail;
	if (detail_level<0) detail_level = 0;
	if (detail_level>=LEVELS_OF_DETAIL) detail_level = LEVELS_OF_DETAIL-1;
	//cout<<"Using detail Level:"<<detail_level<< " extra due to radius: "<<extra_detail<<endl;
	glCallList(glu_objects[detail_level].displayListId);
}


void Sphere::drawMe(bool insideView)
{
	if (glu_objects[0].displayListId == 0) {
		initDisplayLists();
	}
	glColor3f(1.0f, 1.0f, 1.0f);	// Set Color To White
	glPushMatrix();
	glTranslatef(position.getX(), position.getY(), position.getZ());	

	if (insideView){
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE);
		glBlendColor(0,0,0,0.5);
	}


	double rot_amt = getQuatAngle(orient);
	Vector3d rot_axis = getQuatAxis(orient);
	glRotated(rot_amt, rot_axis.getX(),
	          rot_axis.getY(), rot_axis.getZ());	
	
	if (init_radius!=radius){
		float scale_amount = radius/init_radius;
		glScalef(scale_amount,scale_amount,scale_amount);
	}
	
	drawSphere();	

	if (insideView){
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
	}
	
	glPopMatrix();
}

void Sphere::drawShadowVolume() {
	// first draw a circle

	int dist = SDLEngine::getInstance()->getDistanceFromCam(position);
	if (dist>MAX_DIST_MDETAIL) dist = MAX_DIST_MDETAIL;
	if (dist<MIN_DIST_FDETAIL) dist = MIN_DIST_FDETAIL;
	float denom = MAX_DIST_MDETAIL - MIN_DIST_FDETAIL;
	float num  = dist - MIN_DIST_FDETAIL;
	int levels_of_detail = (MAX_SUBDIVISIONS-MIN_SUBDIVISIONS);
	int shadowSubdivisions =  levels_of_detail - ((int)((float)(num/denom)*levels_of_detail)) + 1;
	float rad_diff = radius - init_radius;
	if (rad_diff<0) rad_diff = 0;
	int extra_detail = (int)(rad_diff*((float)levels_of_detail/10.0));
	shadowSubdivisions+=extra_detail;
	//cout<<"Using : "<<shadowSubdivisions<<" shadow subdivisions. Extra due to radius:"<<extra_detail<<endl;
	
	Point* pointsFront = (Point*)malloc(sizeof(Point) * (shadowSubdivisions + 1));
	Point* pointsBottom = (Point*)malloc(sizeof(Point) * (shadowSubdivisions + 1));

	*pointsFront = Point(0.0, 0.0, 0.0);

	// generate points in a circle
	Point* pTemp = pointsFront + 1;
	double c = (2 * PI) / ((double)shadowSubdivisions);
	for (int i = 0; i < shadowSubdivisions; ++i) {
		double c2 = ((double)i) * c;
		*pTemp = Point(radius * SHADOW_FUDGE * cos(c2), 0, radius * SHADOW_FUDGE * -sin(c2));
		++pTemp;
	}

	// create a matrix to shift the points up to the spheres position, then rotate to face the light
	// shift first
	Matrix mat;
	mat.identity();

	// get light position
	Point lightPos = theWorld->getLightPos(0);

	
	if ((lightPos[0] != position.getX()) || (lightPos[2] != position.getZ())) {
		// determine the XZ vector that we will use to rotate the by to aim towards the light
		// first get vector from us to the light in the XZ plane
		Vector xz(lightPos[0] - position.getX(), 0, lightPos[0] - position.getZ());

		// convert vector to a point then rotate it by pi/2, and normalize to get our xz rotation axis
		Point p(xz.x(), xz.y(), xz.z());
		p.transform(matrixRotate(Vector(0, 1.0, 0), 90));

		Vector xzRotationAxis(p.x(), p.y(), p.z());
		xzRotationAxis.normalize();

		// calculate angle to be rotated
		Vector sphereToLight(lightPos[0] - position.getX(), lightPos[1] - position.getY(), lightPos[2] - position.getZ());
		sphereToLight.normalize();
		double angle = sphereToLight * Vector(0.0, 1.0, 0.0);
		angle = acos(angle);
		angle *= (180/PI);
		//printf("Angle: %f\n", angle);
		mat *= matrixRotate(xzRotationAxis, angle);
	}

	mat *= matrixTranslate(position.getX(), position.getY(), position.getZ());

	// apply matrix to all our points
	pTemp = pointsFront;
	for (int i = 0; i < (shadowSubdivisions + 1); ++i) {
		pTemp->transform(mat);
		++pTemp;
	}

	// extrude all the points out
	pTemp = pointsFront;
	Point* pTemp2 = pointsBottom;
	for(int i = 0; i < (shadowSubdivisions + 1); ++i) {
		// create vector from light to point
		Vector v(pTemp->x() - lightPos[0], pTemp->y() - lightPos[1], pTemp->z() - lightPos[2]);
		// normalize, then scale by 200
		v.normalize();
		v *= 200;
		// create new point which is the addition of the existing point and the scaled vector
		*pTemp2 = Point(pTemp->x() + v.x(), pTemp->y() + v.y(), pTemp->z() + v.z());
		pTemp++;
		pTemp2++;
	}


	glPushMatrix();

	// draw the top cap
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 0.0f, 0.0f);

	pTemp = pointsFront;
	for (int i = 0; i < (shadowSubdivisions + 1); ++i) {
		pTemp->glVertex();
		++pTemp;
	}

	(pointsFront+1)->glVertex();
	glEnd();

	// draw the bottom cap
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 0.0f, 0.0f);
	
	// draw center point
	pointsBottom->glVertex();
	// run through the vertices in opposite direction
	pTemp = pointsBottom + shadowSubdivisions;
	for (int i = 0; i < (shadowSubdivisions); ++i) {
		pTemp->glVertex();
		--pTemp;
	}

	// close the fan
	(pointsBottom + shadowSubdivisions)->glVertex();
	glEnd();

	// draw the sides
	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 0.0f, 0.0f);
	
	pTemp = pointsFront + 1;
	pTemp2 = pointsBottom + 1;
	for (int i = 0; i < (shadowSubdivisions); ++i) {
		pTemp->glVertex();
		pTemp2->glVertex();
		++pTemp;
		++pTemp2;
	}

	// close the quads
	(pointsFront+1)->glVertex();
	(pointsBottom+1)->glVertex();
	glEnd();

	glPopMatrix();	

	free(pointsFront);
	free(pointsBottom);
}

/*
void Sphere::checkCollisionWith(Shape* other_shape){
	Sphere* other = (Sphere *)other_shape;
	double mindistance = radius + other->getRadius();
	mindistance*=mindistance;
	//This is 2d collision
	if (position.distanceSquared(other_shape->getPosition()) < (mindistance)){
		Sound::getInstance()->playSound(BALL_COLLISION);
		//assumes other shape is always a sphere for now
		Vector3d other_vel = other_shape->getVelocity();
		double mass_ratio = other_shape->getMass()/mass;
		double dx = other_shape->getPosition().getX() - position.getX();
		double dz = other_shape->getPosition().getZ() - position.getZ();
		double dvx = other_vel.getX()- velocity.getX();
		double dvz = other_vel.getZ()- velocity.getZ();
		//Check div by 0!!!!
		double temp=1.0E-12*fabs(dz);                            
		 if (fabs(dx)<temp) {  
               if (dx<0) dx = -temp;
			   else dx = temp;
        }
		double slope = dz/dx;
		double dvx2 = -2*(dvx+slope*dvz)/((1+slope*slope)*(1+mass_ratio));
		dvx2*=1.1; //extra bouncyness
		velocity.decX(mass_ratio*dvx2);
		velocity.decZ(slope*mass_ratio*dvx2);
		other_vel.incX(dvx2);
		other_vel.incZ(slope*dvx2);
		other_shape->setVelocity(other_vel);
	}
}
*/

/*
void Sphere::checkCollisionWith(Shape* other_shape){
	
	Sphere* other = (Sphere *)other_shape;
	double mindistance = radius + other->getRadius();
	mindistance*=mindistance;
	double e = 1.1; //coefficient of restitution
	
	
	if (position.distanceSquared(other_shape->getPosition()) < (mindistance)){
		Sound::getInstance()->playSound(BALL_COLLISION);
		//assumes other shape is always a sphere for now

		Vector3d other_vel = other_shape->getVelocity();
		double other_mass = other_shape->getMass();

		double m1 = (mass - (e*other_mass))/(mass+other_mass);
		double m2 = (other_mass/(mass+other_mass))*(e+1);
		Vector3d v1_new = (velocity*m1) + (other_vel*m2);

		m1 = (mass/(mass+other_mass))*(e+1);
		m2 = (other_mass - (e*mass))/(mass+other_mass);
		Vector3d v2_new = (velocity*m1) + (other_vel*m2);

		velocity = v1_new;
		other_shape->setVelocity(v2_new);

	}
}
*/



//Collision Detection - Take 3

void Sphere::checkCollisionWith(Shape* other_shape){
	Sphere* other = (Sphere *)other_shape;
	double mindistance = radius + other->getRadius();
	mindistance*=mindistance;
	//This is 3d collision
	double e = BOUNCINESS;
	if (position.distanceSquared(other_shape->getPosition()) < (mindistance+0.1)){
		//cout<<"Before - This: "<<velocity<<" other:"<<other_shape->getVelocity()<<endl;
		Sound::getInstance()->playSound(BALL_COLLISION);
		Vector3d other_vel = other_shape->getVelocity();
		double other_mass = other_shape->getMass();

		Vector3d old_vel = velocity;
		Vector3d normal = position - other_shape->getPosition();
		normal.normalize();
		
		float v1 = normal.dotProduct(velocity);
		float v2 = normal.dotProduct(other_vel);
		
		velocity = velocity - normal.getScaledCopy(v1);
		other_vel = other_vel - normal.getScaledCopy(v2);
		
		double m1 = (mass - (e*other_mass))/(mass+other_mass);
		double m2 = (other_mass/(mass+other_mass))*(e+1);

		velocity = velocity + normal.getScaledCopy((v1*m1)+(v2*m2));

		Vector3d v1_change = normal.getScaledCopy((v1*m1)+(v2*m2)) - normal.getScaledCopy(v1);

		m1 = (mass/(mass+other_mass))*(e+1);
		m2 = (other_mass - (e*mass))/(mass+other_mass);

		other_vel = other_vel + normal.getScaledCopy((v1*m1)+(v2*m2));
		Vector3d v2_change = normal.getScaledCopy((v1*m1)+(v2*m2)) - normal.getScaledCopy(v2);

		//Need to double check this - Triple Collision with Stage
		Vector3d stage_normal1 = theWorld->getStage()->getNormalFor(this);
		Vector3d stage_normal2 = theWorld->getStage()->getNormalFor(other_shape);
		if (!stage_normal1.isZero()) stage_normal1.normalize();
		if (!stage_normal2.isZero()) stage_normal2.normalize();
		if (!stage_normal1.isZero()){
			float force_mag = stage_normal1.dotProduct((normal * normal.dotProduct(other_shape->getVelocity())) *  -1);
			float force_mag2 = stage_normal1.dotProduct(v2_change);
			if (force_mag>0 && (force_mag*VERTICAL_BOUNCINESS)>force_mag2){
				other_vel = other_vel + (stage_normal1 * force_mag * VERTICAL_BOUNCINESS);
				other_vel = other_vel - (stage_normal1 * force_mag2);
			}
		}
		if (!stage_normal2.isZero()){
			float force_mag = stage_normal2.dotProduct((normal * normal.dotProduct(old_vel)) *  -1);
			float force_mag2 = stage_normal2.dotProduct(v1_change);
			if (force_mag>0  && (force_mag*VERTICAL_BOUNCINESS)>force_mag2 ){
				velocity = velocity + (stage_normal2 * force_mag * VERTICAL_BOUNCINESS);
				velocity = velocity - (stage_normal2 * force_mag2);
			}
		}

		other_shape->setVelocity(other_vel);
		//cout<<"After - This: "<<velocity<<" other:"<<other_shape->getVelocity()<<endl;
	}
}




bool Sphere::separateSpheres(Shape* other_shape){
	Sphere* other = (Sphere *)other_shape;
	double mindistance = radius + other->getRadius();
	mindistance*=mindistance;
	if (position.distanceSquared(other_shape->getPosition()) < mindistance){
		Vector3d dir = other_shape->getPosition() - position;
		float mag = sqrt(mindistance) - dir.getMagnitude() + 0.01;
		float mag1 = mag*(other_shape->getMass()/(mass+other_shape->getMass()));
		float mag2 = mag - mag1;
		
		position = position - (dir*mag1);
		other_shape->setPosition(other_shape->getPosition() + (dir*mag2));
		
		/*position = getOldPosition();
		other_shape->setPosition(other_shape->getOldPosition());
		position = position + (velocity*tick_delta);
		other_shape->setPosition(other_shape->getPosition() + other_shape->getVelocity()*tick_delta);
		if(position.distanceSquared(other_shape->getPosition()) < (mindistance)){
			//We went the wrong way, correct + retract
			position = position - (velocity*(tick_delta*2));
			other_shape->setPosition(other_shape->getPosition() - other_shape->getVelocity()*tick_delta*2);
		}*/
		assert(position.distanceSquared(other_shape->getPosition()) > mindistance);
		return true;
	}
	return false;
}


bool Sphere::collidesWith(Shape* other_shape)
{
	Sphere* other = (Sphere *)other_shape;
	double mindistance = radius + other->getRadius();
	mindistance*=mindistance;
	return(position.distanceSquared(other_shape->getPosition()) < (mindistance+0.1));
}


void Sphere::updateState()
{
	Shape::updateState();

	Vector3d rot_axis = calcRotationAxis();
	if (!rot_axis.isZero()) {
		rot_axis.normalize();
	}
	double rot_amt = calcRotationAmt();
	CWTSQuaternion<double> tempQuat = buildQuat(rot_axis, rot_amt);
	orient = tempQuat * orient;
}

// NOT TO BE USED EXTERNALLY FOR PHYSICS ROTATIONS!!
// Purely for displays of rotating spheres (e.g. in menu)
void Sphere::rotateBy(const Vector3d &rotAxis, double rotAmt)
{
	CWTSQuaternion<double> tempQuat = buildQuat(rotAxis, rotAmt);
	orient = tempQuat * orient;	
}

Vector3d Sphere::calcRotationAxis()
{
	// normal from the surface I'm rolling on
	// Eventually, this should be calculated from terrain.
	// Axis of rotation is perpendicular to both ground-normal and to velocity.
	Vector3d groundNormal(0,1,0);	
	return groundNormal.crossProduct(velocity);
}

double Sphere::calcRotationAmt()
{
	double dist_travelled = velocity.getMagnitude();
	return (dist_travelled / getCircumference()) * DEGREES_IN_A_CIRCLE * tick_delta; 	
}

double Sphere::getCircumference() 
{
	return 2*PI*radius;
}




void Sphere::initDisplayLists() {

	int increment = (MAX_SUBDIVISIONS - MIN_SUBDIVISIONS + 1)/LEVELS_OF_DETAIL;
	for (int i=0;i<LEVELS_OF_DETAIL;i++){
		glu_objects[i].displayListId = glGenLists(1);
		glNewList(glu_objects[i].displayListId, GL_COMPILE);
		// Material
		glMaterialfv(GL_FRONT, GL_AMBIENT, appearance.matProps.ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, appearance.matProps.diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, appearance.matProps.specular);
		glMaterialf(GL_FRONT, GL_SHININESS, appearance.matProps.shininess);
		glBindTexture(GL_TEXTURE_2D, appearance.texNum);

		if (appearance.bumpMap) {
			glUniform1iARB(S_UNI_POS[S_UNI_bumpmap], 1.0);
			glUniform1iARB(S_UNI_POS[S_UNI_isSphere], 1.0);

			glActiveTextureARB(GL_TEXTURE1_ARB);
			glBindTexture(GL_TEXTURE_2D, appearance.tex2Num);
			glUniform1iARB(S_UNI_POS[S_UNI_tex1], 1);
		}

		int divisions = MAX_SUBDIVISIONS - (increment*i);
		gluSphere(glu_objects[i].glu_shape, init_radius, divisions, divisions);
		
		if (appearance.bumpMap) {
			// deactivate texture 2
			glActiveTextureARB(GL_TEXTURE1_ARB);
			glDisable(GL_TEXTURE_2D);
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glUniform1iARB(S_UNI_POS[S_UNI_bumpmap], 0.0);
			glUniform1iARB(S_UNI_POS[S_UNI_isSphere], 0.0);
		}
		glEndList();
	}
}

double Sphere::getRadius()
{
	return radius;
}

void Sphere::setRadius(double radius)
{
	setRadius(radius, true);
}

void Sphere::setAppearance(const Appearance &app) 
{
	appearance = app;
	// Clear out the old display list so that the 
	// next draw will automatically update it.

	for (int i=0;i<LEVELS_OF_DETAIL;i++){
		if (glu_objects[i].displayListId != 0) {
			glDeleteLists(glu_objects[i].displayListId, 1);
			glu_objects[i].displayListId = 0;
		}
	}
}

// This one's private, so that outsiders can't control
// value of isRadiusAlreadySet.
void Sphere::setRadius(double radius, bool isRadiusAlreadySet)
{
	if (isRadiusAlreadySet) {
		position.incY(radius - this->radius); // Update y-position
	} else {
		position.setY(radius);
		init_radius = radius;
	}
	this->radius = radius;
	mass = radius*radius*radius; //can leave out the 4/3PI
}

void Sphere::overrideMass(float mass)
{
	this->mass = mass;

}