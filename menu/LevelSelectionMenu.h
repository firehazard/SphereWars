#ifndef LEVELSELECTIONMENU_H_
#define LEVELSELECTIONMENU_H_
#include "Menu.h"

class LevelSelectionMenu : public Menu
{
  public:
	LevelSelectionMenu(GameParameters *params);
	virtual ~LevelSelectionMenu();
	void init();
	Menu *actOnButtonClick(int buttonIndex);
	
  private:
    vector<pair<string,string> > m_levelNamesAndFilenames;
    int m_backButtonIndex;
    
};

#endif /*LEVELSELECTIONMENU_H_*/
