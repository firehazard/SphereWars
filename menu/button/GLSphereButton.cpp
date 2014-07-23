#include "GLSphereButton.h"
#include "../../FontManager.h"
#include "../../sdlheaders.h"
#include "../../SDLEngine.h"
#include "../../Constants.h"

#include <math/matrix4.h>
#include <iostream>
using namespace std;
#define DEFAULT_RADIUS 30

#define INITIAL_ROTATE_AXIS  Vector3d::yAxis
#define INITIAL_ROTATE_AMT   90

#define ROTATE_INC     -0.5
#define ROTATE_AXIS    Vector3d::zAxis


GLSphereButton::GLSphereButton()
{
	memset(&m_appearance, 0, sizeof(Appearance));
	m_appearance.texNum = INVALID_TEXTURE;

	m_radius = DEFAULT_RADIUS;
	m_sphere.rotateBy(INITIAL_ROTATE_AXIS, INITIAL_ROTATE_AMT);
}

GLSphereButton::~GLSphereButton()
{
}

void GLSphereButton::calcSphereCoordinates()
{
	Matrix mvMat, projMat;
	glMatrixMode(GL_MODELVIEW);
	mvMat.glLoadMatrix();
	glMatrixMode(GL_PROJECTION);
	projMat.glLoadMatrix();
	
	Matrix mv_proj_mat = mvMat * projMat;
	Matrix inverse = mv_proj_mat.inverse();
	
	float xFraction = (getX() - SCREEN_WIDTH/2 + m_radius)/((float)(SCREEN_WIDTH/2));
	float yFraction = (SCREEN_HEIGHT/2 - getY() - m_radius)/((float)(SCREEN_HEIGHT/2));
	
	Vector screenPosn(xFraction, yFraction, 0.5); 
	Vector convertedPosn = mv_proj_mat * screenPosn;
	
	glMatrixMode(GL_MODELVIEW);
	m_sphere.setPosition(Vector3d(convertedPosn.x(), convertedPosn.y(), convertedPosn.z()));
	m_sphere.setRadius(getSphereRadiusForResolution(SCREEN_WIDTH, SCREEN_HEIGHT));
}

void GLSphereButton::drawRect(float r, float g, float b, float a)
{
	FontManager::enable2D();
	glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 0.0);
	glColor4f(r,g,b,a);
	glRectf(getX(), SCREEN_HEIGHT - getY(), 
			getX() + getWidth(), SCREEN_HEIGHT - getY() - getHeight());
	
	glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 1.0);
	FontManager::disable2D();	
}

void GLSphereButton::drawMe()
{
	// TODO: Only call when a boolean is false (which gets reset when radius,x,y change)
	calcSphereCoordinates();

	if (isEnabled()) {
		if (m_isHovered) {
			drawRect(0,0.7,0.7,0.6);
		} else {
			drawRect(0,0.5,0.5,0.5);
		}
		m_sphere.rotateBy(ROTATE_AXIS, ROTATE_INC);
	}

	glDisable(GL_DEPTH_TEST);
	m_sphere.drawMe(false);
	glEnable(GL_DEPTH_TEST);
		
	if (!isEnabled()) {
		drawRect(0.6,0.6,0.6,0.6);
	}
		
	

}

void GLSphereButton::setAppearance(const Appearance &app)
{
	m_appearance = app;
	m_sphere.setAppearance(app);
}

void GLSphereButton::setRadius(int radius)
{
	m_radius = radius;	
}

int GLSphereButton::getDefaultWidth()
{
	return DEFAULT_RADIUS * 2;	
}


float GLSphereButton::getSphereRadiusForResolution(const int width, const int height)
{
	if (width == 1024 && height == 768) {
		return 0.05;
	} else if (width == 1280 && height == 1024) {
		return 0.04;
	} else if (width == 1920 && height == 1200) {
		return 0.03;
	} else if (width == 1920 && height == 1200) {
		return 0.02;
	} else if (width == 2560 && height == 1600) {
		return 0.01;
	} else {
		return 0.01;
	}
}
