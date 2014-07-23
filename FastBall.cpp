#include "FastBall.h"
#include "globals.h"
#include "SDLEngine.h"

FastBall::FastBall(const Vector3d &theInitialPosition)
	: Powerup(theInitialPosition)
{
	glu_icon = gluNewQuadric();
	gluQuadricNormals(glu_icon, GL_SMOOTH);
	initDisplayList();
	reset();
	ptype = Powerup::FAST_BALL;
}

FastBall::~FastBall(void)
{
}

void FastBall::reset()
{
	Powerup::reset();
	elapsed_time = 0;
	icon_rot = 0;		
}

void FastBall::initDisplayList()
{
	
	displayListIcon = glGenLists(1);
	glNewList(displayListIcon, GL_COMPILE);

	GLfloat matAmbientDiffuse[] = {10.5f, 10.5f, 10.5f};
	GLfloat matSpecular[] = {1.0f, 1.0f, 1.0f};
	GLfloat matShininess[] = {100.0};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbientDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

	glColor3f(0.5f, 1.0f, 0.5f);
	gluSphere(glu_icon,0.5,20,20);
	
	glEndList();
}


void FastBall::drawMe()
{
	if (owner!=NULL) {
		for (int i=0;i<MOTION_SAMPLES;i++){
			((Sphere *)owner->getShape())->drawAt(prev_positions[i], 1.0-((float)i*(1.0/(float)MOTION_SAMPLES)));
		}

	}
	else {
		glPushMatrix();
		Powerup::drawMe();
		glPushMatrix();
		glRotatef(icon_rot,0,1,0);
		glTranslatef(0,0,1);
		glCallList(displayListIcon);
		glPopMatrix();
		glPopMatrix();
	}
	glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 1.0);
}


#define MAX_ALLOTED_TIME 7
void FastBall::updateState()
{
	Powerup::updateState();

	if (owner==NULL){
		icon_rot+=(tick_delta*1000);
		if (icon_rot>360) icon_rot-=360;
		return;
	}

	if (elapsed_time>MAX_ALLOTED_TIME) return;

	for (int i=MOTION_SAMPLES-1;i>0;i--){
		prev_positions[i] = prev_positions[i-1];
	}
	prev_positions[0] = owner->getShape()->getPosition();

	elapsed_time+=tick_delta;
}

void FastBall::setOwner(Player* player)
{
	Powerup::setOwner(player);
	player->setFastPowerup(true);
	Sound::getInstance()->playSound(FASTBALL_PICKUP);
	elapsed_time = 0;
	for (int i=0;i<MOTION_SAMPLES;i++){
		prev_positions[i] = player->getShape()->getPosition();		
	}
}


bool FastBall::mustDestroy()
{
	if (Powerup::mustDestroy()	|| elapsed_time>MAX_ALLOTED_TIME){
		owner->setFastPowerup(false);
		return true;
	}
	return false;
}


