#include "BigBall.h"
#include "globals.h"
#include "Sound.h"
#include "SDLEngine.h"


BigBall::BigBall(const Vector3d &theInitialPosition)
	: Powerup(theInitialPosition)
{
	glu_icon = gluNewQuadric();
	gluQuadricNormals(glu_icon, GL_SMOOTH);
	initDisplayList();
	reset();
	ptype = Powerup::BIG_BALL;
}

BigBall::~BigBall(void)
{
}

void BigBall::reset()
{
	Powerup::reset();
	done_shrinking = false;
	scale_factor = 0.5;
	is_add_factor = true;	
}
void BigBall::initDisplayList()
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
	gluSphere(glu_icon,1,20,20);
	
	glEndList();
}


void BigBall::drawMe()
{
	if (owner!=NULL) return;
	glPushMatrix();
	Powerup::drawMe();
	glPushMatrix();
	glScalef(scale_factor,scale_factor,scale_factor);
	glCallList(displayListIcon);
	glPopMatrix();
	glPopMatrix();
	glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 1.0);
}


void BigBall::setOwner(Player* player)
{
	Powerup::setOwner(player);
	Sound::getInstance()->playSound(GROW);
	elapsed_time = 0;
	done_shrinking = false;
	first_shrink = true;
	init_radius = ((Sphere *)owner->getShape())->getRadius();
}


#define MAX_SIZE 5
#define MAX_ALLOTED_TIME 7
void BigBall::updateState()
{
	Powerup::updateState();

	if (owner==NULL){
		if (scale_factor>1.8) {
			is_add_factor = !is_add_factor;
			scale_factor = 1.8;
		}
		if (scale_factor<0.5) {
			is_add_factor = !is_add_factor;
			scale_factor = 0.5;
		}

		if (is_add_factor) scale_factor+=(tick_delta);
		else scale_factor-=(tick_delta*4);
		return;
	}
	
	if (elapsed_time>MAX_ALLOTED_TIME && done_shrinking) return;

	Sphere* ball = ((Sphere *)owner->getShape());
	
	if (elapsed_time<MAX_ALLOTED_TIME){
		if ( ball->getRadius() < MAX_SIZE) {
			ball->setRadius(ball->getRadius() + (tick_delta*5));
		}
	}
	else {
		ball->setRadius(ball->getRadius() - (tick_delta*5));
		if (first_shrink) Sound::getInstance()->playSound(SHRINK);
		first_shrink = false;
		if (ball->getRadius()<init_radius){ 
			ball->setRadius(init_radius);
			done_shrinking = true;
		}
	}
	elapsed_time+=tick_delta;
}

bool BigBall::mustDestroy()
{
	if (Powerup::mustDestroy() || done_shrinking){
		Sphere* ball = ((Sphere *)owner->getShape());
		if ( ball->getRadius() != init_radius) {
			ball->setRadius(init_radius);
		}
		return true;
	}
	return false;
}

