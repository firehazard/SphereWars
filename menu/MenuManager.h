#ifndef MENUMANAGER_H_
#define MENUMANAGER_H_
#include "../GameParameters.h"
#include "../FontManager.h"
#include "Menu.h"

class MenuManager
{
  public:
	MenuManager();
	~MenuManager();
	void init();
	
	void drawMe();
	
	// Should take keys, mouse events, and/or joystick events
	void applyUserInteraction();
	void handleMouseClick(int x, int y);
	void handleMouseHover(int x, int y);
	
	const GameParameters *getParams() const  { return &m_params; }
	bool isReadyForGame() const;
  private:
	GameParameters m_params;	
	Menu *m_currentMenu;
	FontManager m_fontMngr;
	FontManager::CachedTexture m_titleTex;
    int m_titleXPos;
    int m_titleYPos;
    
    void drawTitle();    
};

#endif /*ROOTMENU_H_*/
