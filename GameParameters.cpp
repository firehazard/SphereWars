#include "GameParameters.h"
#include "Constants.h"
#include <assert.h>

GameParameters::GameParameters()
{
	// TODO: Init all member data
	m_numPlayers = 0;
}

GameParameters::~GameParameters()
{
}

void GameParameters::setNumPlayers(int numPlayers)
{ 
	m_numPlayers = numPlayers;
	m_playerAppearances.clear();
	
	// Create a dummy appearance to use for populating the vector
	Appearance dummy;
	memset(&dummy, 0, sizeof(Appearance));
	dummy.texNum = INVALID_TEXTURE;
	
	// Initialize a spot for each player's texture
	for (int i = 0; i < numPlayers; i++) {		
		m_playerAppearances.push_back(dummy);
	}
}

int GameParameters::getNumPlayers() const
{
	return m_numPlayers;
}


void GameParameters::setAppearanceForPlayer(int playerNum, const Appearance &app)
{
	assert(playerNum < getNumPlayers());
	assert(playerNum < (int)(m_playerAppearances.size()));
	m_playerAppearances[playerNum] = app;
}

const Appearance &GameParameters::getAppearanceForPlayer(int playerNum) const
{
	return m_playerAppearances[playerNum];
}

void GameParameters::setLevelInfo(const LevelInfo *levelInfo)
{
	m_levelInfo = levelInfo;
}

const LevelInfo *GameParameters::getLevelInfo() const
{
	return m_levelInfo;
}
