#ifndef INSTRUCTIONSMENU_H_
#define INSTRUCTIONSMENU_H_
#include "Menu.h"

class InstructionsMenu : public Menu
{
  public:
	InstructionsMenu(GameParameters *params);
	virtual ~InstructionsMenu();
	void init();
	Menu *actOnButtonClick(int buttonIndex);
  private:
	int initControlsDisplay();
	int addDisabledTextButton(const string &text, int x, int y, bool isHeader);
};

#endif /*INSTRUCTIONSMENU_H_*/
