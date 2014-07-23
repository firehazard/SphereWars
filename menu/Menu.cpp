#include "Menu.h"
#include "button/TextButton.h"

#define TITLE_Y_POS     135
#define TITLE_FONT_SIZE 30
#define TITLE_R         70
#define TITLE_G         190
#define TITLE_B	        70

Menu::Menu(GameParameters *params)
	: m_params(params), 
	  m_lastHoveredButton(NO_BUTTON),
	  m_isTitleValid(false)
{
}

Menu::~Menu()
{
	 for (unsigned i = 0; i < m_buttons.size(); i++) {
		delete m_buttons[i];			
	}
}

void Menu::drawMe()
{
	if (m_isTitleValid) {
		m_titleButton.drawMe();
	}
	for (unsigned i = 0; i < m_buttons.size(); i++) {
		m_buttons[i]->drawMe();			
	}	
} 
	
int Menu::getButtonIndexForPosn(int x, int y)
{
	for (unsigned i = 0; i < m_buttons.size(); i++) {
		if (m_buttons[i]->isInBounds(x,y)) {
			return i;
		}
	}	
	return NO_BUTTON;
}

void Menu::actOnButtonHover(int buttonIndex)
{
	// Only need to do anything if we've changed which button we're hovering over.
	if (buttonIndex != m_lastHoveredButton) {
		if (m_lastHoveredButton != NO_BUTTON) {
			// Un-hover the last button
			m_buttons[m_lastHoveredButton]->setHoverStatus(false);
		}
		if (buttonIndex != NO_BUTTON) {
			m_buttons[buttonIndex]->setHoverStatus(true);
		}
		m_lastHoveredButton = buttonIndex;
	}
}

void Menu::setTitle(const string &text)
{
	m_titleButton.setTextColor(TITLE_R, TITLE_G, TITLE_B);
	m_titleButton.setText(text);
	m_titleButton.setTextSize(TITLE_FONT_SIZE);
	m_titleButton.setX(m_titleButton.getCenteredXPos());
	m_titleButton.setY(TITLE_Y_POS);
	m_isTitleValid = true;
}
