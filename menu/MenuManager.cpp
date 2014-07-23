#include "MenuManager.h"
#include "RootMenu.h"
#include "BallSelectionMenu.h"
#include "NumPlayersMenu.h"
#include "../Constants.h"

#define TITLE                 "SPHERE WARS"
#define TITLE_SIZE            50
#define TITLE_FONT_PATH		  "resources/Ethnocen.ttf"
#define TITLE_DIST_FROM_EDGE  50
#define TITLE_R               120
#define TITLE_G               180
#define TITLE_B               200


MenuManager::MenuManager()
{
	m_titleTex.texture = INVALID_TEXTURE;
	m_titleXPos = 0;
	m_titleYPos = 0;
}

MenuManager::~MenuManager()
{
	if (m_currentMenu) {
		delete m_currentMenu;
	}
	if (m_titleTex.texture != INVALID_TEXTURE) {
		glDeleteTextures(1, &m_titleTex.texture);
	}
}

void MenuManager::init()
{
	m_currentMenu = new RootMenu(&m_params);
	m_currentMenu->init();
	m_fontMngr.setFontSize(TITLE_SIZE);
	m_fontMngr.setFontPath(TITLE_FONT_PATH);
	m_fontMngr.setColor(TITLE_R, TITLE_G, TITLE_B);
	m_titleTex = m_fontMngr.renderText(TITLE);
	m_titleXPos = (int)(0.5 *(SCREEN_WIDTH - m_titleTex.text_w));
	m_titleYPos = SCREEN_HEIGHT - TITLE_DIST_FROM_EDGE - m_titleTex.image_h;	
}

void MenuManager::drawMe()
{	
	drawTitle();
	if (m_currentMenu) {
		m_currentMenu->drawMe();
	}
}

void MenuManager::drawTitle() 
{
	m_fontMngr.drawCachedTexture(m_titleTex, 
	                             m_titleXPos, m_titleYPos);
}

void MenuManager::handleMouseClick(int x, int y)
{
	if (m_currentMenu) {
		// Button has been pressed, and we need to handle it.
		//printf("Handling button click at (%hd, %hd).\n", x, y);
		int buttonIndex = m_currentMenu->getButtonIndexForPosn(x, y);
		if (buttonIndex != NO_BUTTON) {
			//printf("  Button %d was clicked!\n", buttonIndex);
			Menu *newMenu = m_currentMenu->actOnButtonClick(buttonIndex);
			if (newMenu != m_currentMenu) {
				// Moving on to a different menu
				delete m_currentMenu;
				m_currentMenu = newMenu;
				
				if (m_currentMenu != NULL) {
					m_currentMenu->init();
				} // else, menu == NULL, which means we're ready to start the game
			}
		}
	}
}

void MenuManager::handleMouseHover(int x, int y)
{
	if (m_currentMenu) {
		int buttonIndex = m_currentMenu->getButtonIndexForPosn(x, y);
		m_currentMenu->actOnButtonHover(buttonIndex);
	}
}

bool MenuManager::isReadyForGame() const
{
	return m_currentMenu == NULL;
} 

