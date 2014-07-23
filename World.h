#ifndef WORLD_H
#define WORLD_H

#include "Player.h"
#include "Powerup.h"
#include "BigBall.h"
#include "FastBall.h"
#include "Stage.h"
#include "sdlheaders.h"
#include "LevelInfo.h"
#include <vector>
#include "SkyBox.h"
#include <math/point.h>
#include "ParticleEffects.h"
#include "Vector3d.h"

using namespace std;

#define NO_WINNER   (-1)

class World
{

private:
	vector<Player *> players;
	vector<Powerup *> powerups;
	GLuint stageDisplayList;
	Stage* stage;
	SkyBox skyBox;
	bool m_isOnVictoryScreen;
	int m_winnerIndex;
	void detectCollisions();
	void detectGameOver();
		void lighting();
	int numLights;
	vector<Point> lightPositions;
	double lastFireworksTick;
	bool startedVictoryMusic;
	bool drawShadowVolume;

	struct powerupLocation {
		Vector3d position;
		Vector3d rotation_axis;
		float rotation_angle;
		bool occupied;
	};

	
	void drawShadow();
	void drawShadows();
	void clearPlayers();
	void clearPowerups();
	void checkCurrentPowerups();
	vector<ParticleEffects*> particleEffects;
	vector<MissileParticleEffects*> missileParticleEffects;
	void addPowerup(Powerup::PowerupType type);
	
	
	Vector3d pickPowerupLocation(vector<powerupLocation>& vec,  Vector3d& rot_axis, float& rot_angle);

	bool autospin_enabled;

	vector<powerupLocation> missile_locations;
	vector<powerupLocation> bigball_locations;
	vector<powerupLocation> fastball_locations;
	//num powerups in game play
	int num_missiles;
	int num_bigballs;
	int num_fastballs;

	float powerup_time;

public:
	World();
	~World();
	void init();
	void reset();
	void decPowerupNum(Powerup* powerup);

	void rotatePowerupsBy(float angle, Vector3d axis);


	void setNumPlayers(int newNumPlayers);
	void clearPowerupLocation(Powerup* powerup);
	void applyLevelInfo(const LevelInfo *lvlInfo);
	void drawMe(int screenNum, bool missileSteer, bool isGameRunning);
	void updateWorld();
	Stage* getStage();
	Player* getPlayer(unsigned int player_num);
	void createPlayer(int shape_type);
	unsigned int getNumPlayers();
	unsigned int getNumLights();
	Point getLightPos(int lightNum);
	bool playerCollides(Player *player);
	
	bool getAutoSpinEnabled();
	void toggleAutoSpin();


	void steerMissileAttempt(Player* player, Vector3d force);
	void launchMissileAttempt(Player* player, Vector3d direction);
	bool playerLaunchedMissile(Player *player, Vector3d* pos = NULL, Vector3d* vel = NULL);
	void addParticleEffect(ParticleEffects* particleEffect);
	void addMissileParticleEffect(MissileParticleEffects* missileParticleEffect);
	bool isOnVictoryScreen();
	int getWinnerIndex();
	void savePowerupPosition(Powerup::PowerupType type, Vector3d position);
	void toggleShadowVolume();
	bool two_player_mode;
	void updateMusic(bool isPause);
};

#endif
