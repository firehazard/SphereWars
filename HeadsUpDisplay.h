#ifndef HEADSUPDISPLAY_H_
#define HEADSUPDISPLAY_H_

#include "FontManager.h"
#include "Missile.h"
#include "Sphere.h"
#include <vector>
#include <map>
using namespace std;

class HeadsUpDisplay
{
  public:
	HeadsUpDisplay(int screenWidth, int screenHeight);
	~HeadsUpDisplay();
	
	/**
	 * Draws the heads-up display
	 */
	void drawMe();
	void drawMeSplitScreen(int playerNum);
	void setNumPlayers(unsigned numPlayers);
	void prepareVictoryMessage(int winnerIndex);
	void clearVictoryMessage();
	
  private:  
    FontManager m_fontMngr;
    unsigned m_scrnWidth;
    unsigned m_scrnHeight;
    vector<short> m_xPosns;
    vector<short> m_yPosns;
    TextureCache m_cachedTextures;
    FontManager::CachedTexture m_victoryMessage;
    
	short calcYPositionForPlayer(int playerIndex);
	short calcXPositionForPlayer(int playerIndex);
	void displayPlayerInfo(int playerIndex, bool isSplitScreen);
	int displayLabel(int playerIndex, int x, int y);
	int displayLives(int playerIndex, int x, int y, bool isSplitScreen);
	int displayPowerup(int playerIndex, int x, int y, bool isSplitScreen);
	void displayVictoryMessage();	
	void drawCircleHelper(int x, int y, int radius);
	void drawRadar(int player_num);

	void drawRadarCircle(int player_num, int radarCtrX, int radarCtrY);

	void drawRadarPoint(Vector3d position, int player_num, int radarCtrX, int radarCtrY);
	short getViewportHeight(bool isSplit);
	short getViewportWidth(bool isSplit);
};

#endif /*HEADSUPDISPLAY_H_*/
