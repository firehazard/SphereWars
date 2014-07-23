#include "HeadsUpDisplay.h"
#include "Utils.h"     // for loadBitmap
#include "Constants.h" // for INVALID_TEXTURE
#include "globals.h"   // for theWorld 
#include "SDLEngine.h"
#include <sstream>
#include <assert.h>

#define VICTORY_MSG_DISTANCE_FROM_EDGE  50
#define VICTORY_SIZE  40


#define SIDE_BUFFER 20
#define HUD_HEIGHT   80
#define HUD_LINESPACING 3
#define TEXT_CORRECTION 10

#define FONT_R 150
#define FONT_G 255
#define FONT_B 255
#define FONT_PATH "resources/ComicSansMS-Bold.ttf"

#define MISSILE_ICON_RADIUS   15
#define LIFE_ICON_RADIUS      10
#define LIFE_ICON_ANGLE_STEP   (2 * PI) / 50
#define LIFE_ICON_OFFSET      (2 * LIFE_ICON_RADIUS + 15)
#define MISSILE_SEPARATION    10
#define MAX_DISPLAY_MISSILES  5

#define RADAR_RANGE           120.0f

#define RADAR_CTR_X           100
#define RADAR_CTR_Y           100
#define RADAR_RADIUS           60
#define RADAR_BLEEP_RADIUS           5



HeadsUpDisplay::HeadsUpDisplay(int scrnWidth, int scrnHeight)
 : m_scrnWidth(scrnWidth), 
   m_scrnHeight(scrnHeight)
{
	// Can't query theWorld for numPlayers yet, 'cause theWorld isn't yet initialized
	setNumPlayers(0);
	
	m_fontMngr.setColor(FONT_R, FONT_G, FONT_B);
	m_fontMngr.setFontPath(FONT_PATH);
	
	m_victoryMessage.texture = INVALID_TEXTURE;
}

HeadsUpDisplay::~HeadsUpDisplay()
{
	clearVictoryMessage();
}

void HeadsUpDisplay::setNumPlayers(unsigned numPlayers)
{
	m_xPosns.clear();
	for (unsigned i = 0; i < numPlayers; i++) {
		m_xPosns.push_back(calcXPositionForPlayer(i));
		m_yPosns.push_back(calcYPositionForPlayer(i));
	}
}

void HeadsUpDisplay::drawMe()
{
	assert(theWorld->getNumPlayers() == m_xPosns.size());
	
	if (m_victoryMessage.texture == INVALID_TEXTURE) {
		// No victory message to draw => game must be running. Draw player HUD info
		for (unsigned i = 0; i < theWorld->getNumPlayers(); i++) {
			displayPlayerInfo(i, false);	
		}
	} else {
		displayVictoryMessage();
	}
}

void HeadsUpDisplay::drawMeSplitScreen(int playerNum)
{
	displayPlayerInfo(playerNum, true);
	drawRadar(playerNum);
}

void HeadsUpDisplay::drawRadar(int player_num)
{
	int radarCtrX = getViewportWidth(true) - SIDE_BUFFER - RADAR_RADIUS;
	int radarCtrY = SIDE_BUFFER + RADAR_RADIUS;
	FontManager::enable2D();
	glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 0.0);
	Vector3d cam_dir = SDLEngine::getInstance()->getCameraDirection();
	double zcomp = -cam_dir.getZ()/cam_dir.getMagnitude();
	double angle = acosf(zcomp);
	angle*=(180.0/PI);
	if (cam_dir.getX()>0) {
		angle = 360.0 - angle;
	}

	Vector3d pos = theWorld->getPlayer(player_num)->getShape()->getPosition();
	drawRadarCircle(player_num, radarCtrX, radarCtrY);
	Vector3d distance;
	glColor4f(0.2f,1.0f,0.2f,0.6);
	for (int i=0;i<(int)theWorld->getNumPlayers();i++){
		if (i!=player_num){
			distance = theWorld->getPlayer(i)->getShape()->getPosition() - pos;
			if (!distance.isZero() && distance.getMagnitude()<RADAR_RANGE){
				distance.setY(0);
				distance.rotateBy(angle, Vector3d::yAxis);
				distance.setY(-distance.getZ());
				distance.setZ(0);
				distance = distance.getScaledCopy(1/RADAR_RANGE);
				drawRadarPoint(distance, player_num, radarCtrX, radarCtrY);
			}
		}
	}
		
	glColor4f(0.0f,0.0f,0.0f,0.6);
	drawRadarPoint(Vector3d::zero, player_num, radarCtrX, radarCtrY);
	FontManager::disable2D();
	glEnable(GL_DEPTH_TEST);
	glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 1.0);
}




//Draw a semi-transparent circle
void HeadsUpDisplay::drawRadarCircle(int player_num, int radarCtrX, int radarCtrY)
{

	glColor4f(0.5f,0.5f,1.0f,0.4);
	drawCircleHelper(radarCtrX, radarCtrY, RADAR_RADIUS);
}


//Assumes position goes from -1 to 1 in both X/Y coordinates
void HeadsUpDisplay::drawRadarPoint(Vector3d position, int player_num, int radarCtrX, int radarCtrY)
{
	//cout<<"Drawing dot at "<<position<<endl;
	drawCircleHelper(radarCtrX + position.getX()*RADAR_RADIUS,
	                 radarCtrY + position.getY()*RADAR_RADIUS,
	                 RADAR_BLEEP_RADIUS);
	                 
}




void HeadsUpDisplay::prepareVictoryMessage(int winnerIndex)
{
	m_fontMngr.setFontSize(VICTORY_SIZE);
	if (winnerIndex == NO_WINNER) {
		m_victoryMessage = m_fontMngr.renderText("It's a draw!");
	} else {
		stringstream stream;
		stream << "Player " << (winnerIndex + 1) << " Wins!";
		m_victoryMessage = m_fontMngr.renderText(stream.str().c_str());
	}
}

void HeadsUpDisplay::clearVictoryMessage()
{
	if (m_victoryMessage.texture != INVALID_TEXTURE) {
		glDeleteTextures(1, &(m_victoryMessage.texture));
		m_victoryMessage.texture = INVALID_TEXTURE;
	}
}

void HeadsUpDisplay::displayVictoryMessage()
{
	short x = (SCREEN_WIDTH - m_victoryMessage.text_w) / 2;
	short y = (SCREEN_HEIGHT - VICTORY_MSG_DISTANCE_FROM_EDGE - m_victoryMessage.image_h);  
	m_fontMngr.drawCachedTexture(m_victoryMessage, x, y);
}


void HeadsUpDisplay::displayPlayerInfo(int playerIndex, bool isSplitScreen)
{
	// TODO: Set color with something like this
	// m_fontMngr.setColor(player.getColor())
	int x,y;
	if (isSplitScreen) {
		x = SIDE_BUFFER;
		y = HUD_HEIGHT; 
	} else {
		x = m_xPosns[playerIndex];
		y = m_yPosns[playerIndex];
	}
	y -= displayLabel(playerIndex, x, y) - HUD_LINESPACING;
	y += TEXT_CORRECTION;
	y -= displayLives(playerIndex, x, y, isSplitScreen) - HUD_LINESPACING;
	displayPowerup(playerIndex, x, y, isSplitScreen);
}

int HeadsUpDisplay::displayLabel(int playerIndex, int x, int y)
{
	stringstream stream;
	stream << "Player " << (playerIndex + 1);
	FontManager::CachedTexture cachedTex = 
	   lookupOrCreateTexture(m_fontMngr, m_cachedTextures, stream.str());
	m_fontMngr.drawCachedTexture(cachedTex, x, y);						
	return cachedTex.text_h;	
}

int HeadsUpDisplay::displayLives(int playerIndex, int x, int y, bool isSplitScreen)
{
	FontManager::enable2D();
	glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 0.0);
		
	glColor3f(0.5f,0.5f,1.0f);

	x += LIFE_ICON_RADIUS;
	for (int i = 0; i < theWorld->getPlayer(playerIndex)->getNumLives(); i++) {
		drawCircleHelper(x, y, LIFE_ICON_RADIUS);
		x += LIFE_ICON_OFFSET;
	}	
	glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 1.0);
	FontManager::disable2D();
	glEnable(GL_DEPTH_TEST);
	return 20;
}

void HeadsUpDisplay::drawCircleHelper(int x, int y, int radius)
{	
	glBegin(GL_POLYGON);
         for(float angle = 0; angle < 2 * PI; angle += LIFE_ICON_ANGLE_STEP){
            glVertex2f(x + cos(angle) * radius, y + sin(angle) * radius);
         }
      glEnd();
}

int HeadsUpDisplay::displayPowerup(int playerIndex, int x, int y, bool isSplitScreen)
{
	int mcount = theWorld->getPlayer(playerIndex)->getMissileCount();

	for (int i = 0; i < mcount && i < (MAX_DISPLAY_MISSILES); i++) {
		FontManager::enable2D();
		glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 0.0);
		
		glColor3f(0.5f,1.0f,0.5f);
		glBegin(GL_POLYGON);
			glVertex2f(x,y);
			glVertex2f(x,y-10);
			glVertex2f(x+20,y-5);
		glEnd();
		
		glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 1.0);
		FontManager::disable2D();
		x += MISSILE_ICON_RADIUS + MISSILE_SEPARATION;
	}
	glEnable(GL_DEPTH_TEST);
	if (mcount>0) {
		return MISSILE_ICON_RADIUS;
	} else {
		return 0;
	}
}

short HeadsUpDisplay::calcXPositionForPlayer(int playerIndex)
{
	int range = m_scrnWidth - SIDE_BUFFER * 2;
	int widthPerPlayer = range / theWorld->getNumPlayers();  	
	return SIDE_BUFFER + playerIndex * (widthPerPlayer);
}

short HeadsUpDisplay::calcYPositionForPlayer(int playerIndex)
{
	return HUD_HEIGHT;
}

short HeadsUpDisplay::getViewportWidth(bool isSplit)
{
	if (isSplit && theWorld->getNumPlayers() > 2) {
		return SCREEN_WIDTH / 2;
	} else {
		return SCREEN_WIDTH;
	} 
}

short HeadsUpDisplay::getViewportHeight(bool isSplit)
{
	if (isSplit && theWorld->getNumPlayers() > 1) {
		return SCREEN_HEIGHT / 2;
	} else {
		return SCREEN_HEIGHT;
	}
}

