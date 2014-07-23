#ifndef GAMEPARAMETERS_H_
#define GAMEPARAMETERS_H_

#include "sdlheaders.h"
#include "Appearance.h"
#include "LevelInfo.h"
#include <vector>
using namespace std;

class GameParameters
{
 public:
	GameParameters();
	~GameParameters();
	
	void setNumPlayers(int numPlayers);
	int getNumPlayers() const;
	
	void setAppearanceForPlayer(int playerNum, const Appearance &app);
	const Appearance &getAppearanceForPlayer(int playerNum) const;
	
	void setLevelInfo(const LevelInfo *levelInfo);
	const LevelInfo *getLevelInfo() const;
	
 private:
	int m_numPlayers;
	vector<Appearance> m_playerAppearances;
	const LevelInfo *m_levelInfo;
	// Level
	// Player's ball-texture choices
	// Player controls -- keyboard vs joystick?
	//  (not easy to make customizeable with our current framework)

	// Possibly:
	//  graphics quality? (low,med,hi?)
	//  Stage Activity on/off (lets user enable/disable spontaneous 
	//                         autospin activations, etc?)
};

#endif /*GAMEPARAMETERS_H_*/
