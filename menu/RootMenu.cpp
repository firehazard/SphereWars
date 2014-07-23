#include "RootMenu.h"
#include "NumPlayersMenu.h"
#include "BallSelectionMenu.h"
#include "InstructionsMenu.h"
#include "button/TextButton.h"
#include "../SDLEngine.h"
#include <assert.h>

#define NEWGAME_BTN_INDEX 0
#define INFO_BTN_INDEX    1
#define QUIT_BTN_INDEX    2

#define TITLE             "Main Menu"
#define NEWGAME_BTN_TEXT  "New Game"
#define INFO_BTN_TEXT     "Controls"
#define QUIT_BTN_TEXT     "Quit"


RootMenu::RootMenu(GameParameters *params)
	: Menu(params)
{
}

RootMenu::~RootMenu()
{
}

void RootMenu::init()
{
	TextButton *txtBut;
	int yPos = MENU_START_Y;
	

	txtBut = new TextButton();
	txtBut->setText(NEWGAME_BTN_TEXT);	
	txtBut->setX(txtBut->getCenteredXPos());
	txtBut->setY(yPos);
	m_buttons.push_back(txtBut);
	yPos += txtBut->getHeight() + BUTTON_SEP;

	txtBut = new TextButton();
	txtBut->setText(INFO_BTN_TEXT);	
	txtBut->setX(txtBut->getCenteredXPos());
	txtBut->setY(yPos);
	m_buttons.push_back(txtBut);
	yPos += txtBut->getHeight() + BUTTON_SEP;

	txtBut = new TextButton();
	txtBut->setText(QUIT_BTN_TEXT);	
	txtBut->setX(txtBut->getCenteredXPos());
	txtBut->setY(yPos);
	m_buttons.push_back(txtBut);
	yPos += txtBut->getHeight() + BUTTON_SEP;
	
	setTitle(TITLE);
}


Menu *RootMenu::actOnButtonClick(int buttonIndex)
{
	switch(buttonIndex) {
		case NEWGAME_BTN_INDEX:
			return new NumPlayersMenu(m_params);
		case INFO_BTN_INDEX:
			return new InstructionsMenu(m_params);			
			break;
		case QUIT_BTN_INDEX:
			 SDLEngine::quit();
		default:			
			assert(0);
	}
	return this; // DEBUG
}

