#include "Missile.h"
#include "SDLEngine.h"
#include "Constants.h"
#include "globals.h"
#include "Sound.h"
#include <assert.h>
#include <math/point.h>
#include <math/matrix4.h>
#include <vector>
#include <iostream>
#include "SmokeParticleEffects.h"

using namespace std;

#define MISSILE_VELOCITY 40
#define MISSILE_RADIUS 2
#define MISSILE_BOUNCINESS 10

Missile::Missile(const Vector3d &theInitialPosition)
	: Powerup(theInitialPosition)
{
	baseRadius = 1;
	height = 3;
	ptype = Powerup::MISSILE;

	for (int i=0;i<LEVELS_OF_DETAIL;i++){
		glu_missiles[i].glu_shape = gluNewQuadric();
		glu_closings[i] = gluNewQuadric();
		gluQuadricNormals(glu_missiles[i].glu_shape, GL_SMOOTH);
		gluQuadricNormals(glu_closings[i], GL_SMOOTH);
	}
	
	particles = new MissileParticleEffects(this);
	theWorld->addMissileParticleEffect(particles); 
	initDisplayLists();
	reset();
}

void Missile::reset()
{
	Powerup::reset();
	velocity = Vector3d(0,0,0);
	owner = NULL;
	launched = false;
	collided = false;
	powerup_angle = 0;
	particles->reset();
}

void Missile::initDisplayLists()
{
	int increment = (MAX_SUBDIVISIONS - MIN_SUBDIVISIONS + 1)/LEVELS_OF_DETAIL;
	int divisions = 0;
	for (int i=0;i<LEVELS_OF_DETAIL;i++){
		glu_missiles[i].displayListId = glGenLists(1);
		glNewList(glu_missiles[i].displayListId, GL_COMPILE);
		
		GLfloat matAmbientDiffuse[] = {10.5f, 10.5f, 10.5f};
		GLfloat matSpecular[] = {1.0f, 1.0f, 1.0f};
		GLfloat matShininess[] = {100.0};

		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbientDiffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
		glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

		glColor3f(0.5f, 1.0f, 0.5f);
		divisions = MAX_SUBDIVISIONS - (increment*i);
		//cout<<"Creating missile cone with: "<<divisions<<" subdivisions"<<endl;
		gluCylinder(glu_missiles[i].glu_shape, baseRadius, 0, height, divisions, divisions);
		gluDisk(glu_closings[i],0,1,divisions,divisions); 
		
		glEndList();
	}
}


Missile::~Missile(void)
{
	particles->setExpired(true);
}

float powerup_angle = 0;

void Missile::setOwner(Player *player)
{
	Powerup::setOwner(player);
	Sound::getInstance()->playSound(MISSILE_PICKUP);
		
	if (owner != NULL) {
		owner->incMissileCount();
	}	
}

void Missile::drawMe()
{
	if (!launched && owner!=NULL) return;
	
	glPushMatrix();
	
	
	Powerup::drawMe();
	
	bool inside = false;

	if (!velocity.isZero()){
		float angle = acosf(Vector3d(0,0,1).dotProduct(velocity.getUnitVector())) * (180/PI);
		Vector3d axis = Vector3d(0,0,1).crossProduct(velocity.getUnitVector());
		glRotatef(angle,axis.getX(),axis.getY(),axis.getZ());
	}

	if (owner==NULL){
		glRotatef(-90,1,0,0);
		glPushMatrix();
		glRotatef(powerup_angle,0,1,0);
		glTranslatef(0,0,-1.5);
	} else {
		inside = owner->getTransparent();
	}
		
	if (inside){
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_ONE, GL_ONE);

	
	}

	int detail_level = Powerup::getDetailLevel(position);
	//cout<<"Drawing missile cone with: "<<detail_level<<" detail level"<<endl;
	glCallList(glu_missiles[detail_level].displayListId);

	if (inside){
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}

	if (owner==NULL) glPopMatrix();


	glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 1.0);

	glPopMatrix();
}

bool Missile::hasLaunched()
{
	return launched;
}

void Missile::drawShadowVolume() {
	if (!launched) return;
	// first draw a circle
	std::vector<Point> pointsFront;
	pointsFront.push_back(Point(0.0, 0.0, 0.0));

	int dist = SDLEngine::getInstance()->getDistanceFromCam(position);
	if (dist>MAX_DIST_MDETAIL) dist = MAX_DIST_MDETAIL;
	if (dist<MIN_DIST_FDETAIL) dist = MIN_DIST_FDETAIL;
	float denom = MAX_DIST_MDETAIL - MIN_DIST_FDETAIL;
	float num  = dist - MIN_DIST_FDETAIL;
	int levels_of_detail = (MAX_SUBDIVISIONS-MIN_SUBDIVISIONS);
	int subdivisions =  levels_of_detail - ((int)((float)(num/denom)*levels_of_detail)) + 1;

	//cout<<"Drawing missile shadows with: "<<subdivisions<<" subdivisions"<<endl;
	// generate points in a circle
	double c = (2 * PI) / ((double)subdivisions);
	for (int i = 0; i < subdivisions; ++i) {
		double c2 = ((double)i) * c;
		pointsFront.push_back(Point(Vector(baseRadius * cos(c2), baseRadius * -sin(c2), 0), Vector(0,0,-1)));
	}

	Vector base(0,0,1);
	Vector3d unitVelocity = velocity.getUnitVector();
	
	float angle = acos(base * Vector(unitVelocity.getX(), 0, unitVelocity.getZ())) * (180 / PI);
	//printf("Angle: %f\n", angle);
	Vector axis = xProduct(Vector(0,0,1), Vector(unitVelocity.getX(), 0, unitVelocity.getZ()));
	axis.normalize();
	//printf("Axis: %f %f %f\n", axis.x(), axis.y(), axis.z());

	// create a matrix to rotate the points then shift them up to the right position
	// shift first
	Matrix mat;
	mat.identity();
	mat *= matrixRotate(axis, -angle);	
	mat *= matrixTranslate(position.getX(), position.getY(), position.getZ());

	// get light position
	Point lightPos = theWorld->getLightPos(0);

	// apply matrix to all our points
	std::vector<Point>::iterator cur = pointsFront.begin();
	std::vector<Point>::iterator end = pointsFront.end();

	while (cur != end) {
		(*cur).transform(mat);
		++cur;
	}

	// extrude all the points out
	std::vector<Point> pointsBottom;
	cur = pointsFront.begin();
	end = pointsFront.end();
	while (cur != end) {
		// create vector from light to point
		Vector v((*cur).x() - lightPos[0], (*cur).y() - lightPos[1],(*cur).z() - lightPos[2]);
		// normalize, then scale by 200
		v.normalize();
		v *= 200;
		// create new point which is the addition of the existing point and the scaled vector
		pointsBottom.push_back(Point((*cur).x() + v.x(), (*cur).y() + v.y(), (*cur).z() + v.z()));
		++cur;
	}

	cur = pointsFront.begin();
	end = pointsFront.end();

	glPushMatrix();
	// draw the top cap
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 0.0f, 0.0f);

	while (cur != end) {
		(*cur).glNormal();
		(*cur).glVertex();
		++cur;
	}

	pointsFront[1].glNormal();
	pointsFront[1].glVertex();
	glEnd();

	// draw the bottom cap
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 0.0f, 0.0f);
	
	// draw center point
	pointsBottom[0].glVertex();
	// run through the vertices in opposite direction
	for (int i = (pointsBottom.size() - 1); i > 0; --i) {
		pointsBottom[i].glVertex();
	}

	// close the fan
	pointsBottom[pointsBottom.size() - 1].glVertex();
	glEnd();

	// draw the sides
	glBegin(GL_QUAD_STRIP);
	glColor3f(1.0f, 0.0f, 0.0f);
	
	for (int i = 1; i < pointsFront.size(); ++i) {
		pointsFront[i].glVertex();
		pointsBottom[i].glVertex();
	}

	// close the quads
	pointsFront[1].glVertex();
	pointsBottom[1].glVertex();
	glEnd();

	// create points for the triangle
	std::vector<Point> triangle;
	triangle.push_back(Point(Vector(baseRadius, 0, 0), Vector(0, 1, 0)));
	triangle.push_back(Point(Vector(-baseRadius, 0, 0), Vector(0, 1, 0)));
	triangle.push_back(Point(Vector(0, 0, height), Vector(0, 1, 0)));

	// rotate the triangle
	for (int i = 0; i < triangle.size(); ++i) {
		triangle[i].transform(mat);
	}

	// extrude all the points out
	std::vector<Point> triangleBottom;
	cur = triangle.begin();
	end = triangle.end();
	while (cur != end) {
		// create vector from light to point
		Vector v((*cur).x() - lightPos[0], (*cur).y() - lightPos[1],(*cur).z() - lightPos[2]);
		// normalize, then scale by 200
		v.normalize();
		v *= 200;
		// create new point which is the addition of the existing point and the scaled vector
		triangleBottom.push_back(Point((*cur).x() + v.x(), (*cur).y() + v.y(), (*cur).z() + v.z()));
		++cur;
	}

	// draw the top face
	glBegin(GL_TRIANGLES);
	triangle[0].glVertex();
	triangle[1].glVertex();
	triangle[2].glVertex();
	glEnd();

	// draw the bottom face
	glBegin(GL_TRIANGLES);
	triangleBottom[2].glVertex();
	triangleBottom[1].glVertex();
	triangleBottom[0].glVertex();
	glEnd();

	// draw the sides
	glBegin(GL_QUAD_STRIP);
	triangle[0].glVertex();
	triangleBottom[0].glVertex();
	triangle[1].glVertex();
	triangleBottom[1].glVertex();
	triangle[2].glVertex();
	triangleBottom[2].glVertex();
	triangle[0].glVertex();
	triangleBottom[0].glVertex();
	glEnd();

	glPopMatrix();	
}

//return true if missile must be destroyed
void Missile::updateState()
{
	Powerup::updateState();
	
	if (owner==NULL){
		powerup_angle+=(tick_delta*200);
		if (powerup_angle>360) powerup_angle-=360;
	}
	
	if (launched){
		particles->updateState();
		position = position + (velocity * tick_delta);
		if (theWorld->getStage()->isMissileTouchingStage(this)){
			createCollisionParticleEffects(position.toVector());
			collided =  true;
		}
		else {
			for (int i=0;i<theWorld->getNumPlayers();i++){
				Shape* shape = theWorld->getPlayer(i)->getShape();
				if (shape!=owner->getShape()){
					checkCollisionWith(shape); 
				}
			}
		}
	}
}



void Missile::checkCollisionWith(Shape* other_shape)
{
	
	Sphere* other = (Sphere *)other_shape;
	float radius = MISSILE_RADIUS;
	double mindistance = radius + other->getRadius();
	mindistance*=mindistance;
	float mass = 10;
	double e = MISSILE_BOUNCINESS;
	if (position.distanceSquared(other_shape->getPosition()) < (mindistance)){
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
		m1 = (mass/(mass+other_mass))*(e+1);
		m2 = (other_mass - (e*mass))/(mass+other_mass);

		other_vel = other_vel + normal.getScaledCopy((v1*m1)+(v2*m2));

		other_shape->setVelocity(other_vel);
		createCollisionParticleEffects(position.toVector());
		collided = true;
	}
}


void Missile::launch(Player* player, Vector3d direction)
{
	if (player!=owner || launched) return;
	launched = true;
	owner->decMissileCount();
	position = owner->getShape()->getPosition();
	Vector3d normal = theWorld->getStage()->getNormalFor(owner->getShape());
	velocity = (direction.getUnitVector())*MISSILE_VELOCITY + owner->getShape()->getVelocity();
	if (!normal.isZero()) {
		velocity.setY(-1* ((normal.getX()*velocity.getX()) + (normal.getZ()*velocity.getZ()))/normal.getY());
	}
	velocity.getCopyWithMagnitude(MISSILE_VELOCITY + owner->getShape()->getVelocity().getMagnitude());
	owner->getShape()->applyForce((velocity.getCopyWithMagnitude(SHOT_BACK/owner->getShape()->getMass()))*-1);
	Sound::getInstance()->playSound(MISSILE_LAUNCH);
}


float Missile::getRadius()
{
	return MISSILE_RADIUS;
}

void Missile::steer(Player* player, Vector3d force)
{
	if (player!=owner || !launched) return;
	velocity = (velocity + force).getCopyWithMagnitude(MISSILE_VELOCITY);
}

#define OUTSIDE_DISTANCE 200
bool Missile::mustDestroy()
{
	if (Powerup::mustDestroy()){
		owner->clearMissileCount();
		return true;
	}
	if  (collided || (position.getZ()<-OUTSIDE_DISTANCE || position.getZ()>OUTSIDE_DISTANCE 
					  || position.getX()<-OUTSIDE_DISTANCE || position.getX()>OUTSIDE_DISTANCE 
					  || position.getY()<-OUTSIDE_DISTANCE || position.getY()>OUTSIDE_DISTANCE)){

		Sound::getInstance()->playSound(MISSILE_EXPLODE);
		return true;
	}
	return false;
}

bool Missile::playerLaunched(Player* player)
{
	return (player==owner && launched);
}

Vector3d Missile::getVelocity()
{
	return velocity;
}

void Missile::createCollisionParticleEffects(Vector location) {
	theWorld->addParticleEffect(new SmokeParticleEffects(75, location, 6));
}
