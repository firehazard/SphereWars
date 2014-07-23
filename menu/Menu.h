#ifndef MENU_H_
#define MENU_H_
#include "../GameParameters.h"
#include "button/Button.h"
#include "button/TextButton.h"
#include <vector>
using namespace std;

#define NO_BUTTON (-1)
#define BUTTON_SEP 8

#define MENU_START_Y  180

class Menu
{
  public:
	Menu(GameParameters *params);
	virtual ~Menu();
	virtual void init()   {}
	void drawMe();
	
	int getButtonIndexForPosn(int x, int y);
	virtual Menu *actOnButtonClick(int buttonIndex) {return NULL;}
	void actOnButtonHover(int buttonIndex);
	
  protected:
  	vector<Button *> m_buttons;
  	GameParameters *m_params;
  	int m_lastHoveredButton;
  	
  	void setTitle(const string &text);
  
  private:	
	TextButton m_titleButton;
	bool m_isTitleValid;
};

#endif /*MENU_H_*/
