#include "InstructionsMenu.h"
#include "RootMenu.h"

#define INSTR_R      230
#define INSTR_G      230
#define INSTR_B      230

#define INSTR_X_POSN 30
#define INSTR_INDENT_BELOW_HEADER 10
#define HEADER_TEXT_SIZE 26
#define STANDARD_TEXT_SIZE 20
#define BETWEEN_SECTION_DIST 40

InstructionsMenu::InstructionsMenu(GameParameters *params)
	: Menu(params)
{
}

InstructionsMenu::~InstructionsMenu()
{
}

void InstructionsMenu::init()
{
	TextButton *txtBut;
	int yPos;
	
	setTitle("Controls");
	
	yPos = initControlsDisplay();

	// Init back button.
	txtBut = new TextButton();
	txtBut->setText("Back");
	txtBut->setX(txtBut->getCenteredXPos());
	txtBut->setY(yPos);
	m_buttons.push_back(txtBut);
	yPos += txtBut->getHeight() + BUTTON_SEP;
	
}

int InstructionsMenu::initControlsDisplay()
{
	int maxYPos = 0;
	int yPos = MENU_START_Y;
	int xPos = INSTR_X_POSN;

	yPos += addDisabledTextButton("Player 1 Keyboard Controls:", xPos, yPos, true);
	xPos += INSTR_INDENT_BELOW_HEADER;
	yPos += addDisabledTextButton("Arrow keys - move", xPos, yPos, false);
	yPos += addDisabledTextButton("Right Control - shoot missile", xPos, yPos, false);
	yPos += addDisabledTextButton("Right Alt - steer missile", xPos, yPos, false);
	yPos += addDisabledTextButton("1 - inside view (when split-screen)", xPos, yPos, false);
	
	xPos = INSTR_X_POSN; // Reset x position for Joystick controls
	yPos += BETWEEN_SECTION_DIST;
	yPos += addDisabledTextButton("Players 1 - 4 Joystick Controls:", xPos, yPos, true);
	xPos += INSTR_INDENT_BELOW_HEADER;
	yPos += addDisabledTextButton("Control Pad - move", xPos, yPos, false);
	yPos += addDisabledTextButton("Button 1 - shoot missile", xPos, yPos, false);
	yPos += addDisabledTextButton("Button 2 - steer missile", xPos, yPos, false);
	yPos += addDisabledTextButton("Button 3 - inside view", xPos, yPos, false);
	yPos += addDisabledTextButton("Start Button - pause", xPos, yPos, false);
	
	maxYPos = yPos;

	
	// SECOND COLUMN
	// -------------

	// Reset height for player 2 controls 
	yPos = MENU_START_Y;
	xPos = SCREEN_WIDTH / 2 + INSTR_X_POSN;
	yPos += addDisabledTextButton("Player 2 Keyboard Controls:", xPos, yPos, true);
	xPos += INSTR_INDENT_BELOW_HEADER;
	yPos += addDisabledTextButton("A/S/D/W - move", xPos, yPos, false);
	yPos += addDisabledTextButton("Left Control - shoot missile", xPos, yPos, false);
	yPos += addDisabledTextButton("Left Alt - steer missile", xPos, yPos, false);
	yPos += addDisabledTextButton("2 - inside view (when split-screen)", xPos, yPos, false);
	
	yPos += BETWEEN_SECTION_DIST;
	xPos = SCREEN_WIDTH / 2 + INSTR_X_POSN;
	yPos += addDisabledTextButton("General Keyboard Controls:", xPos, yPos, true);
	xPos += INSTR_INDENT_BELOW_HEADER;
	yPos += addDisabledTextButton("p - pause", xPos, yPos, false);
	yPos += addDisabledTextButton("m - toggle split screen mode", xPos, yPos, false);
	yPos += addDisabledTextButton("1,2,3,4 - inside view", xPos, yPos, false);
	yPos += addDisabledTextButton("0  - display shadow projections", xPos, yPos, false);
	if (yPos > maxYPos) {
		maxYPos = yPos;	
	}
	return maxYPos;
}

int InstructionsMenu::addDisabledTextButton(const string &text, int x, int y, bool isHeader)
{
	TextButton *txtBut = new TextButton();
	txtBut->setText(text);	
	txtBut->setX(x);
	txtBut->setY(y);
	txtBut->setEnabled(false);
	txtBut->setTextColor(INSTR_R, INSTR_G, INSTR_B);
	if (isHeader) {
		txtBut->setTextSize(HEADER_TEXT_SIZE);	
	} else {
		txtBut->setTextSize(STANDARD_TEXT_SIZE);
	}
	m_buttons.push_back(txtBut);
	return txtBut->getHeight() + BUTTON_SEP;	
}

Menu *InstructionsMenu::actOnButtonClick(int buttonIndex)
{
	return new RootMenu(m_params);
}
