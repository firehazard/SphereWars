#ifndef GLSPHEREBUTTON_H_
#define GLSPHEREBUTTON_H_

#include "Button.h"
#include "../../SDLEngine.h"
#include "../../sdlheaders.h"
#include "../../Sphere.h"

class GLSphereButton : public Button
{
  public:
	GLSphereButton();
	~GLSphereButton();

	void drawMe();
  	int getWidth()    	{ return 2 * m_radius; }
  	int getHeight() 	{ return 2 * m_radius; }
	const Appearance &getAppearance() { return m_appearance; }
	
	void setAppearance(const Appearance &appearance);
	void setRadius(int radius);
  
  	static int getDefaultWidth();
  
  private:
  	Appearance m_appearance;
  	int m_radius;
  	Sphere m_sphere;
  	
  	void calcSphereCoordinates();
  	void drawRect(float r, float g, float b, float a);
  	static float getSphereRadiusForResolution(int width, int height);
};

#endif /*GLSPHEREBUTTON_H_*/
