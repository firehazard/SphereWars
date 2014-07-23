#include "Powerup.h"
#include "globals.h"
#include "SDLEngine.h"
#include "Missile.h"
#include "BigBall.h"
#include "FastBall.h"
#include <assert.h>
#include "PowerupParticleEffects.h"

#define OUTER_SPHERE_RADIUS 2

const string Powerup::s_powerupNames[] = {"missile", "bigball", "fastball"};

// Static functions
// ----------------
Powerup::PowerupType Powerup::getTypeForName(const string &powerupName)
{
	// Linear lookup -- not optimal, but there are few types of powerups
	// and this only needs to be called when loading a level.
	unsigned i;
	for (i = 0; i < NUM_POWERUP_TYPES; i++) {
		if (powerupName == s_powerupNames[i]) {
			break;
		}
	}
	return (PowerupType)i;
}

const string &Powerup::getNameForType(Powerup::PowerupType type)
{
	return s_powerupNames[type];
}		

Powerup *Powerup::buildPowerupOfType(Powerup::PowerupType type, const Vector3d &thePosition)
{
	switch(type) {
		case MISSILE:
			return new Missile(thePosition);
		case BIG_BALL:
			return new BigBall(thePosition);
		case FAST_BALL:
			return new FastBall(thePosition);
		default:
			cerr << "ERROR: Unknown powerup type index: #" << type << endl;
			assert(0);
	}
}

// Non-Static Functions
// --------------------
Powerup::Powerup(const Vector3d &theInitialPosition)
	: initialPosition(theInitialPosition) 
{
	// Sanity check on powerup type names -- make sure we have the right number.
	assert(sizeof(s_powerupNames) / sizeof(string) == NUM_POWERUP_TYPES);
	
	for (int i=0;i<LEVELS_OF_DETAIL;i++){
		powerup_boxes[i].glu_shape = gluNewQuadric();
		gluQuadricNormals(powerup_boxes[i].glu_shape, GL_SMOOTH);
	}
	
	ptype = NUM_POWERUP_TYPES;
	initDisplayList();
	reset();
}

void Powerup::reset()
{
	position = initialPosition;
	owner = NULL;
	powerup_color = 0;
	is_add_color = true;
}


void Powerup::initDisplayList()
{
	int increment = (MAX_SUBDIVISIONS - MIN_SUBDIVISIONS + 1)/LEVELS_OF_DETAIL;
	int divisions = 0;
	for (int i=0;i<LEVELS_OF_DETAIL;i++){

		powerup_boxes[i].displayListId = glGenLists(1);

		glNewList(powerup_boxes[i].displayListId, GL_COMPILE);
		glPushMatrix();
	

		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		//glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_ONE, GL_ONE);
		
		divisions = MAX_SUBDIVISIONS - (increment*i);

		gluSphere(powerup_boxes[i].glu_shape, OUTER_SPHERE_RADIUS, divisions, divisions);

		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		glPopMatrix();

		glEndList();
	}
}


Powerup::~Powerup(void)
{
}

Vector3d Powerup::getPosition()
{
	return position;
}

void Powerup::setOwner(Player* player)
{
	owner = player;
}


void Powerup::drawMe()
{
	glTranslatef(position.getX(), position.getY(), position.getZ());
	glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 0.0);
	if (owner==NULL){
		glPushMatrix();	
		glColor4f(1-powerup_color,0,powerup_color,1);
		int detail_level = getDetailLevel(position);
		//cout<<"Powerup_box Using detail Level:"<<detail_level<<endl;
		glCallList(powerup_boxes[detail_level].displayListId);
		glPopMatrix();
	}
}

void Powerup::rotateBy(float angle, Vector3d axis)
{
	position = initialPosition;
	position.rotateBy(-angle,axis);
}

int Powerup::getDetailLevel(Vector3d pos)
{
	int dist = SDLEngine::getInstance()->getDistanceFromCam(pos);
	if (dist>MAX_DIST_MDETAIL) dist = MAX_DIST_MDETAIL;
	if (dist<MIN_DIST_FDETAIL) dist = MIN_DIST_FDETAIL;
	int detail_level = 0; //0 means most details
	float denom = MAX_DIST_MDETAIL - MIN_DIST_FDETAIL;
	float num  = dist - MIN_DIST_FDETAIL;
	detail_level = (int)((float)(num/denom)* LEVELS_OF_DETAIL);
	if (detail_level<0) detail_level = 0;
	if (detail_level>=LEVELS_OF_DETAIL) detail_level = LEVELS_OF_DETAIL-1;
	return detail_level;
}


bool Powerup::mustDestroy()
{
	if (owner!=NULL && owner->getDeath()){
		return true;
	}
	return false;
}


bool Powerup::collidesWith(Shape *other_shape)
{
	Sphere* other = (Sphere *)other_shape;
	double mindistance = OUTER_SPHERE_RADIUS + other->getRadius();
	mindistance*=mindistance;
	return  (position.distanceSquared(other_shape->getPosition()) < (mindistance));
}


void Powerup::drawShadowVolume()
{


}

Vector3d Powerup::getInitialPosition()
{
	return initialPosition;
}

void Powerup::updateState()
{
	if (owner==NULL) {
		if (powerup_color>1){
			is_add_color=!is_add_color;
			powerup_color = 1;
		}
		if (powerup_color<0){
			is_add_color=!is_add_color;
			powerup_color = 0;
		}	
		if (is_add_color) powerup_color+=tick_delta;
		else powerup_color-=tick_delta;

		for (int i=0;i<theWorld->getNumPlayers();i++){
			Shape* shape = theWorld->getPlayer(i)->getShape();
			if (collidesWith(shape)) {
				theWorld->decPowerupNum(this);
				theWorld->clearPowerupLocation(this);
				setOwner(theWorld->getPlayer(i));
				Sphere* sphere = dynamic_cast<Sphere*>(shape);
				theWorld->addParticleEffect(new PowerupParticleEffects(4, 2, sphere->getRadius() * 4, 32, theWorld->getPlayer(i), 0.5, 6));
			}
		}
	}

}

Powerup::PowerupType Powerup::getType()
{
	return ptype;
}

