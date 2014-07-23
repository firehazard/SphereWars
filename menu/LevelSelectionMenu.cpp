#include "LevelSelectionMenu.h"
#include "BallSelectionMenu.h"
#include "../LevelInfo.h"
#include <assert.h>
#define LEVEL_LIST_FILE "levels/levels.txt"

#define BACK_BUTTON_BUFFER 15

LevelSelectionMenu::LevelSelectionMenu(GameParameters *params)
	: Menu(params)
{
	m_backButtonIndex = -1;
}

LevelSelectionMenu::~LevelSelectionMenu()
{
}


void LevelSelectionMenu::init()
{
	TextButton *txtBut;
	int yPos = MENU_START_Y;
	
	if (!LevelInfo::loadLevelList(LEVEL_LIST_FILE, m_levelNamesAndFilenames)) {
		cerr << "Error loading level list from file \"" << LEVEL_LIST_FILE << "\"." << endl;			
	}
	int i;
	for (i= 0; i < (int)m_levelNamesAndFilenames.size(); i++) {
		const string &name = m_levelNamesAndFilenames[i].first;

		txtBut = new TextButton();
		txtBut->setText(name);	
		txtBut->setX(txtBut->getCenteredXPos());
		txtBut->setY(yPos);		
		m_buttons.push_back(txtBut);
		yPos += txtBut->getHeight() + BUTTON_SEP;
	}
	
	yPos += BACK_BUTTON_BUFFER;
	// Init back button
	m_backButtonIndex = i;	
	txtBut = new TextButton();
	txtBut->setText("Back");	
	txtBut->setX(txtBut->getCenteredXPos());
	txtBut->setY(yPos);
	m_buttons.push_back(txtBut);
	yPos += txtBut->getHeight() + BUTTON_SEP;
	
	setTitle("Select Level:");
}


Menu *LevelSelectionMenu::actOnButtonClick(int buttonIndex)
{
	for (int i = 0; i < (int)m_levelNamesAndFilenames.size(); i++) {
		if (buttonIndex == i) {
			LevelInfo *levelInfo = new LevelInfo();
			if (levelInfo->loadFromFile(m_levelNamesAndFilenames[buttonIndex].second)) {
				m_params->setLevelInfo(levelInfo);
				return NULL; // Done with all menus!
			} else {
				// Crap, there was an error loading level.
				// Stay on this screen, to let user pick a different level or go back.
				delete levelInfo;
				return this;
			}
		}
	}
	if (buttonIndex == m_backButtonIndex) {
		return new BallSelectionMenu(m_params, m_params->getNumPlayers() - 1);
	}
	
	// something went wrong... just return current menu
	assert(0);	
	return this;
}
