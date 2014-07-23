#ifndef ROOTMENU_H_
#define ROOTMENU_H_

#include "Menu.h"

class RootMenu : public Menu
{
  public:
	RootMenu(GameParameters *params);
	~RootMenu();
	void init();
	Menu *actOnButtonClick(int buttonIndex);
	
};

#endif /*ROOTMENU_H_*/
