#include "NumPlayersMenu.h"
#include "RootMenu.h"
#include "BallSelectionMenu.h"
#include "button/TextButton.h"
#include "../Constants.h"
#include <string>
#include <sstream>
#include <assert.h>
using namespace std;

// 1-player option not to be used in production game
// but leaving it here in case we want to use it for
// debugging game using only one sphere.
#define PLAYERS_2_BTN_INDEX   0
#define PLAYERS_3_BTN_INDEX   (PLAYERS_2_BTN_INDEX+1)
#define PLAYERS_4_BTN_INDEX   (PLAYERS_3_BTN_INDEX+1)
#define BACK_BTN_INDEX        (PLAYERS_4_BTN_INDEX+1)


NumPlayersMenu::NumPlayersMenu(GameParameters *params)
 : Menu(params)
{
}

NumPlayersMenu::~NumPlayersMenu()
{
}

void NumPlayersMenu::init()
{
	TextButton *txtBut;
	int yPos = MENU_START_Y;
	
	
	for (int i = PLAYERS_2_BTN_INDEX; i <= PLAYERS_4_BTN_INDEX; i++) {
		stringstream stream;
		stream << (i+MIN_NUM_PLAYERS) << " Player";
		if (i != 0) {
			stream << "s";
		}
		
		txtBut = new TextButton();
		txtBut->setText(stream.str());	
		txtBut->setX(txtBut->getCenteredXPos());
		txtBut->setY(yPos);		
		m_buttons.push_back(txtBut);
		yPos += txtBut->getHeight() + BUTTON_SEP;
	}
	// Init back button	
	txtBut = new TextButton();
	txtBut->setText("Back");	
	txtBut->setX(txtBut->getCenteredXPos());
	txtBut->setY(yPos);
	m_buttons.push_back(txtBut);
	yPos += txtBut->getHeight() + BUTTON_SEP;
	
	setTitle("Select Number Of Players:");
}

Menu *NumPlayersMenu::actOnButtonClick(int buttonIndex)
{
	switch(buttonIndex) {
		case BACK_BTN_INDEX:
			return new RootMenu(m_params);
		case PLAYERS_2_BTN_INDEX:
		case PLAYERS_3_BTN_INDEX:
		case PLAYERS_4_BTN_INDEX:
			// TODO: Set this in gameParameters
			m_params->setNumPlayers(buttonIndex - PLAYERS_2_BTN_INDEX + MIN_NUM_PLAYERS);
			return new BallSelectionMenu(m_params);
		default:			
			assert(0);
	}
	return this; // DEBUG
}
