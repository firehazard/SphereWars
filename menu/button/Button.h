#ifndef BUTTON_H_
#define BUTTON_H_

#include "../../Constants.h"

// TODO: take isEnabled() into account

class Button
{
  public:
	Button();
	virtual ~Button();
	
	virtual void drawMe();
	
	void setHoverStatus(bool flag)  { m_isHovered = flag; }
	
	// X and Y coordinates of upper-left corner, with (0,0) being at the 
	// upper-left corner.
	void setX(int x) { m_x = x;    }
	void setY(int y) { m_y = y;    }
	int getX()       { return m_x; }
	int getY()       { return m_y; }
	
	virtual int getWidth()   {return 0;}
	virtual int getHeight()  {return 0;}	
	
	bool isEnabled()              { return m_isEnabled; }
	void setEnabled(bool enabled) { m_isEnabled = enabled; }		
	
	// TODO: Functions for determining whether a mouse-point is in bounds	
	bool isInBounds(int x, int y);
	
	int getCenteredXPos() {  return (int)(0.5 * (SCREEN_WIDTH - getWidth()));	}	
	int getCenteredYPos() {  return (int)(0.5 * (SCREEN_WIDTH - getHeight()));	}	
	
  protected:
	bool m_isEnabled;
	bool m_isHovered;
	int m_x;
	int m_y;
};

#endif /*BUTTON_H_*/
