#ifndef NUMPLAYERSMENU_H_
#define NUMPLAYERSMENU_H_
#include "Menu.h"

class NumPlayersMenu : public Menu
{
  public:
	NumPlayersMenu(GameParameters *params);
	~NumPlayersMenu();
	void init();
	Menu *actOnButtonClick(int buttonIndex);
	
  private:
  	
};

#endif /*NUMPLAYERSMENU_H_*/
