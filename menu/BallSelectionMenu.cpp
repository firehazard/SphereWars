#include "BallSelectionMenu.h"
#include "NumPlayersMenu.h"
#include "LevelSelectionMenu.h"
#include "button/GLSphereButton.h"
#include "button/TextButton.h"
#include "../SDLEngine.h"
#include <sstream>
using namespace std;

#define SPHERE_BUTTON_SPACING 10

BallSelectionMenu::BallSelectionMenu(GameParameters *params, int curPlayer)
	: Menu(params), m_currentPlayer(curPlayer)
{
	m_backButtonIndex = -1;
}

BallSelectionMenu::~BallSelectionMenu()
{
}

void BallSelectionMenu::init()
{		
	setupButtons();
	updateTitle();
}

void BallSelectionMenu::setupButtons()
{
	GLSphereButton *button;
	vector<Appearance> appearances = SDLEngine::getSphereAppearances();
	unsigned numAppearances = appearances.size();
	m_backButtonIndex = appearances.size();
	
	if (numAppearances == 0) {
		fprintf(stderr, "Error: No skins found to let players choose from.");
		exit(1);
	}	

	// Draw buttons for however many textures we have, 
	// centered and equally spaced in one row across the screen
	int xRange = (numAppearances * GLSphereButton::getDefaultWidth()) + 
	         	((numAppearances - 1) * SPHERE_BUTTON_SPACING);
	int inc = GLSphereButton::getDefaultWidth() + SPHERE_BUTTON_SPACING;
	int x = (SCREEN_WIDTH - xRange) / 2;
	int y =	MENU_START_Y;

	for (unsigned i = 0; i < appearances.size(); i++) {
	    Appearance &app = appearances[i];
	    // create button for texture
		button = new GLSphereButton();
		button->setX(x);
		button->setY(y);
		button->setAppearance(app);
		button->setEnabled(true);
		
		m_buttons.push_back(button);
		m_textureToButton[app.texNum] = button;
		
	    x += inc;
	}

	y += inc;
	TextButton *backButton = new TextButton();
	backButton->setText("Back");	
	backButton->setX(backButton->getCenteredXPos());
	backButton->setY(y);
	m_buttons.push_back(backButton);
}

void BallSelectionMenu::updateTitle()
{
	stringstream stream;
	stream << "Select Skin for Player " << (m_currentPlayer + 1) << ":";
	setTitle(stream.str());	
}

Menu *BallSelectionMenu::actOnButtonClick(int buttonIndex)
{	
	Appearance app;
	if (buttonIndex == m_backButtonIndex) {
		if (m_currentPlayer == 0) { 
			return new NumPlayersMenu(m_params);
		}
		// Step back one player
		m_currentPlayer--;
		
		// Re-enable texture button for (the new) current player's texture
		app = m_params->getAppearanceForPlayer(m_currentPlayer);
		m_textureToButton[app.texNum]->setEnabled(true);
		updateTitle();
	} else {
		// Set Appearance
		app = ((GLSphereButton *)((m_buttons[buttonIndex])))->getAppearance();
		m_params->setAppearanceForPlayer(m_currentPlayer, app);
		m_buttons[buttonIndex]->setEnabled(false);
		m_currentPlayer++;
		if (m_currentPlayer == m_params->getNumPlayers()) {
			return new LevelSelectionMenu(m_params);
		}
		updateTitle();
	}
	return this;
}
