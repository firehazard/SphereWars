#ifndef BALLSELECTIONMENU_H_
#define BALLSELECTIONMENU_H_

#include "Menu.h"
#include "../sdlheaders.h"
#include "../Appearance.h"
#include <map>
using namespace std;

class BallSelectionMenu : public Menu
{
  public:
	BallSelectionMenu(GameParameters *params, int curPlayer = 0);
	~BallSelectionMenu();
  	void init();
	void setCurrentPlayer(int currentPlayer);
	Menu *actOnButtonClick(int buttonIndex);
	
	
  private:
    int m_currentPlayer;
	map<GLuint, Button*> m_textureToButton;
	int m_backButtonIndex;
	void setupButtons();
	void updateTitle();
};

#endif /*BALLSELECTIONMENU_H_*/
