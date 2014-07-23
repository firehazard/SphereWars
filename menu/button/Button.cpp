#include "Button.h"
#include "../../sdlheaders.h"
#include "../../FontManager.h" // for enable2d()
#include "../../SDLEngine.h"

#define HOVER_RED    0.4f
#define HOVER_GREEN  0.6f
#define HOVER_BLUE   0.2f
#define HOVER_ALPHA  0.5f

#define HOVER_HIGHLIGHTING 0

Button::Button()
	: m_isEnabled(true), m_isHovered(false)
{
}

Button::~Button()
{
}

void Button::drawMe()
{
	if (HOVER_HIGHLIGHTING && m_isHovered) {	
		FontManager::enable2D();
		glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 0.0);

		// Draw translucent highlight quad behind button
		glColor4f(HOVER_RED, HOVER_GREEN, HOVER_BLUE, HOVER_ALPHA);
		glRectf(getX(), SCREEN_HEIGHT - getY(), 
				getX() + getWidth(), SCREEN_HEIGHT - getY() - getHeight());
		
		glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 1.0);
		FontManager::disable2D();
	}
}

bool Button::isInBounds(int x, int y)
{
	if(isEnabled()) {
		if (x >= getX() && x <= getX() + getWidth() &&
			y >= getY() && y <= getY() + getHeight()) {
				return true;	 	
		}
	}
	return false;
}
