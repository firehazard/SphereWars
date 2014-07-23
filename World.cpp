#include "World.h"
#include "Sphere.h"
#include "SDLEngine.h"
#include <assert.h>
#include "SmokeParticleEffects.h"
#include "FireworksParticleEffects.h"
#include <math/vector3.h>
#include <algorithm>

#define DROPIN_HEIGHT 40
#define PLAYER_SEPARATION 10

World::World(void)
{
	stage = new Stage();
	lightPositions.push_back(Point(0.0, 50.0, 0.0));
	m_isOnVictoryScreen = false;
	m_winnerIndex = NO_WINNER;
	lastFireworksTick = 0;
	startedVictoryMusic = false;
	num_missiles = num_bigballs = num_fastballs = 0;
	stageDisplayList = 0;
	drawShadowVolume = false;
	autospin_enabled = false;
	powerup_time = 0;
	two_player_mode = false;
}

World::~World(void)
{
	vector<ParticleEffects*>::iterator cur = particleEffects.begin();
	vector<ParticleEffects*>::iterator end = particleEffects.end();
	while (cur != end) {
		delete (*cur);
		++cur;
	}

	for (vector<MissileParticleEffects*>::iterator cur = missileParticleEffects.begin(); cur != missileParticleEffects.end(); ++cur) {
		delete(*cur);
		++cur;
	}

	for (vector<MissileParticleEffects*>::iterator it = missileParticleEffects.begin(); it != missileParticleEffects.end();) {
		delete(*it);
		it = missileParticleEffects.erase(it);
	}


	clearPlayers();
	clearPowerups();
	if (stage) {
		delete stage;
	}
	
}


void World::init() {
	skyBox.init();
}

void World::reset() 
{
	m_isOnVictoryScreen = false;
	m_winnerIndex = NO_WINNER;
	clearPlayers();
	clearPowerups();
	powerup_time = 0;

	// clear particles
	for (vector<ParticleEffects*>::iterator it = particleEffects.begin(); it != particleEffects.end();) {
		delete (*it);
		it = particleEffects.erase(it);
	}

	for (vector<MissileParticleEffects*>::iterator it = missileParticleEffects.begin(); it != missileParticleEffects.end();) {
		delete(*it);
		it = missileParticleEffects.erase(it);
	}


	if (stage) {
		delete stage;
	}
	stage = new Stage();
	startedVictoryMusic = false;
	drawShadowVolume = false;
	autospin_enabled = false;
	stageDisplayList = 0;
	two_player_mode = false;
}

void World::updateMusic(bool isPause)
{
	if (isPause){
					Mix_HaltMusic();
	 }
	 else {
		 if (two_player_mode){
			Sound::getInstance()->loopBackgroundMusic("resources/cool2.mid");
		 }
		 else {
			Sound::getInstance()->loopBackgroundMusic("resources/cool.mid");
		}
	 }

}

void World::clearPlayers()
{
 	for (unsigned i = 0; i < players.size(); i++) {
 		delete players[i];
 	} 	
 	players.clear();
}

void World::clearPowerups()
{
 	for (unsigned i = 0; i < powerups.size(); i++) {
 		delete powerups[i];
 	} 
	missile_locations.clear();
	bigball_locations.clear();
	fastball_locations.clear();
	num_missiles = 0;
	num_fastballs = 0;
	num_bigballs = 0;
 	powerups.clear();
}

void World::setNumPlayers(int newNumPlayers)
{
	assert(newNumPlayers >= 0); 
	// Note: O is a valid # of players, because we draw a world
	// with 0 players in the background while the menu is displayed. 
	
	clearPlayers(); // Remove all old players
	
	for (int i=0; i< newNumPlayers; i++){
		createPlayer(i);
	}
}

// Needs to be called AFTER setNumPlayers.
void World::applyLevelInfo(const LevelInfo *lvlInfo)
{
	// Set stage surfaces 	
	stage->setSurfaces(lvlInfo->getSurfaces());
	if (lvlInfo->isGrass()) {
		stage->toggleGrass();
	}

	// Set player spawn points
	const vector<Vector3d> &spawnPts = lvlInfo->getSpawnPoints();
	assert(spawnPts.size() >= players.size());	
	for (int i=0; i < (int)players.size(); i++) {
		players[i]->getShape()->setPosition(spawnPts[i]);
		players[i]->setSpawnPoint(spawnPts[i]);
	}
	
	// Set powerups
	const vector<pair<Powerup::PowerupType, Vector3d> > lvlPowerups = lvlInfo->getPowerups();
	for (int i=0; i < (int)lvlPowerups.size(); i++) {
		savePowerupPosition(lvlPowerups[i].first, lvlPowerups[i].second);
		addPowerup(lvlPowerups[i].first);
	}
	
	// Set skybox info
	if (lvlInfo->getSkyBoxFaces().size() == 1) {
		const SkyBoxInfo &sbInfo = lvlInfo->getSkyBoxFaces()[0];
		skyBox.setTextureAndRepetitions(sbInfo.texture, sbInfo.xReps, sbInfo.yReps);
	} else {
		for (int i=0; i < (int)lvlInfo->getSkyBoxFaces().size(); i++) {
			const SkyBoxInfo &sbInfo = lvlInfo->getSkyBoxFaces()[i];
			skyBox.setTextureAndRepetitions(sbInfo.texture, sbInfo.xReps, sbInfo.yReps, i);
		}
		skyBox.commitChanges();
	}	
}

bool World::getAutoSpinEnabled()
{
	return autospin_enabled;
}

void World::toggleAutoSpin()
{
	autospin_enabled = !autospin_enabled;
}

void World::savePowerupPosition(Powerup::PowerupType type, Vector3d position)
{
	powerupLocation loc;
	loc.occupied = false;
	loc.position = position;
	loc.rotation_angle = 0;
	loc.rotation_axis = Vector3d(0,0,0);
	switch (type) {
		case Powerup::MISSILE :
			missile_locations.push_back(loc);
			break;
		case Powerup::BIG_BALL:
			bigball_locations.push_back(loc);
			break;
		case Powerup::FAST_BALL:
			fastball_locations.push_back(loc);
			break;

	}

}

//We can generalize this function to add stuff whenever we want it to... very simple for now
#define TIME_BETWEEN_CHECKS 10
void World::checkCurrentPowerups()
{
	powerup_time+=tick_delta;
	if (powerup_time>TIME_BETWEEN_CHECKS){
		if (num_missiles<missile_locations.size()) addPowerup(Powerup::MISSILE);
		if (num_fastballs<fastball_locations.size()) addPowerup(Powerup::FAST_BALL);
		if (num_bigballs<bigball_locations.size()) addPowerup(Powerup::BIG_BALL);
		powerup_time = 0;
	}


}


void World::addPowerup(Powerup::PowerupType type)
{
	Powerup *powerup;
	Vector3d location;
	Vector3d rot_axis;
	float rot_angle;
	switch (type) {
		case Powerup::MISSILE :
			location = pickPowerupLocation(missile_locations, rot_axis, rot_angle);
			powerup = Powerup::buildPowerupOfType(type, location);
			num_missiles++;
			break;
		case Powerup::BIG_BALL:
			location = pickPowerupLocation(bigball_locations, rot_axis, rot_angle);
			powerup = Powerup::buildPowerupOfType(type, location);
			num_bigballs++;
			break;
		case Powerup::FAST_BALL:
			location = pickPowerupLocation(fastball_locations, rot_axis, rot_angle);
			powerup = Powerup::buildPowerupOfType(type, location);
			num_fastballs++;
			break;
	}
	powerups.push_back(powerup);
	if (!rot_axis.isZero()) {
		powerup->rotateBy(rot_angle, rot_axis);
	}
}



//We can modify this function to decide where missiles appear (within missile locations)
Vector3d World::pickPowerupLocation(vector<powerupLocation>& vec, Vector3d& rot_axis, float& rot_angle)
{
	Vector3d location;
	for (int i=0;i<vec.size();i++)
	{
		if (!vec[i].occupied){
			vec[i].occupied = true;
			location = vec[i].position;
			rot_axis = vec[i].rotation_axis;
			rot_angle = vec[i].rotation_angle;
			return location;
		}
	}
	assert(0);
}

//better not move the powerup before calling this (like the missile)
void World::clearPowerupLocation(Powerup* powerup)
{
	vector<powerupLocation>* vec;
	switch (powerup->getType()) {
		case Powerup::MISSILE :
			vec = &missile_locations;
			break;
		case Powerup::BIG_BALL:
			vec = &bigball_locations;
			break;
		case Powerup::FAST_BALL:
			vec = &fastball_locations;
			break;
		default:
			assert(0);
			break;
	}
	for (int i=0;i<vec->size();i++)
	{
		if (powerup->getInitialPosition() == (*vec)[i].position){
			(*vec)[i].occupied = false;
			return;
		}
	}
	assert(0);
}



void World::decPowerupNum(Powerup* powerup)
{
	switch (powerup->getType()) {
		case Powerup::MISSILE :
			num_missiles--;
			break;
		case Powerup::BIG_BALL:
			num_bigballs--;
			break;
		case Powerup::FAST_BALL:
			num_fastballs--;
			break;
		default:
			assert(0);
			break;
	}
}


void World::createPlayer(int player_num)
{
	Player *newPlayer = new Player(player_num);
	newPlayer->setShape(0);
	// Player spawn point will be set later, after level has been loaded.
	players.push_back(newPlayer);
}

// In victory screen, this *just* updates particleEffects (for fireworks)
void World::updateWorld()
{
	if (m_isOnVictoryScreen) {
		// have we played the victory sound?
		if (!startedVictoryMusic) {
			// is the old music still playing?
			if (!Sound::getInstance()->isMusicPlaying()) {
				Sound::getInstance()->playSound(VICTORY);
				startedVictoryMusic = true;
			}
		}
	
		// launch fireworks if we're on the victory screen
		if ((now - lastFireworksTick) > 0.03) {
			lastFireworksTick = now;

			if (Utils::random() < 0.1) {
				Point camPoint = SDLEngine::getInstance()->getCameraLocation();
				float camAngle = Utils::getAngleInRadians(Vector3d(1, 0, 0), Vector3d(0,0,0), Vector3d(camPoint.x(), 0, camPoint.z()));
				if (camPoint.z() < 0.0) {
					camAngle = (2 * PI) - camAngle;
				}

				//printf("Cam: %f ", camAngle);

				float angle = camAngle + (PI / 2) + (Utils::random() * (PI));
				//printf("FW: %f\n", angle);
				float distance = 25 + 10 * Utils::random();
				particleEffects.push_back(new FireworksParticleEffects(200, Vector(distance * cos(angle), -30, distance * sin(angle)), 2.0, 6));
				Sound::getInstance()->playSound(FIREWORKS_LAUNCH);
			}

		}
	}

	vector<ParticleEffects*>::iterator cur = particleEffects.begin();
	vector<ParticleEffects*>::iterator end = particleEffects.end();
	while (cur != end) {
		(*cur)->updateState();
		++cur;
	}
	
	cur = particleEffects.begin();
	end = particleEffects.end();

	for (vector<ParticleEffects *>::iterator it = particleEffects.begin(); it!=particleEffects.end();){
		if ((*it)->isExpired())  {
			delete (*it);
			it = particleEffects.erase(it);
		} else {
			it++;
		}
	}

	if (!isOnVictoryScreen()) {
		/** only check if its expired here, if it is then remove and delete, let the missile class do the updating **/
		for (vector<MissileParticleEffects*>::iterator it = missileParticleEffects.begin(); it != missileParticleEffects.end();) {
			if ((*it)->getExpired()) {
				delete(*it);
				it = missileParticleEffects.erase(it);
			} else {
				++it;
			}
		}

		int num_alive = 0;
		for (unsigned int i=0;i<players.size();i++){
			players[i]->updateState();
			if (players[i]->getNumLives() > 0) num_alive++;
		}

		if (num_alive==2 && !two_player_mode && players.size()>2){
			two_player_mode = true;
			Sound::getInstance()->loopBackgroundMusic("resources/cool2.mid");
			toggleAutoSpin();
			
		}

		checkCurrentPowerups();

		for (vector<Powerup *>::iterator it=powerups.begin(); it!=powerups.end();){
			(*it)->updateState();
			if ((*it)->mustDestroy())  {
				//decPowerupNum((*it));
				delete (*it);
				it = powerups.erase(it);
			}
			else {
				it++;
			}
		}

		
		
		//by now we are doing erasing powerups, we should reset death for all live players (powerup might have
		//died before the player actually died).
		for (int i=0; i<players.size();i++){
			if (players[i]->getNumLives()>0) players[i]->resetDeath();
		}
		
		detectCollisions();
		stage->updateState();
	
		detectGameOver();
	}	
}

void World::detectCollisions()
{
	//Update speeds based on collision
	for (unsigned i=0;i<players.size();i++){
		for (unsigned j=i+1;j<players.size();j++){
			players[i]->getShape()->checkCollisionWith(players[j]->getShape());	
		}
	}
	//Separate overlapping Spheres (should optimize to only check if there was a collision)
	bool restart = true;
	int count = 0;
	while (restart && count<(pow((double)players.size(),2))) {
		restart = false;
		for (unsigned i=0;i<players.size();i++){
			for (unsigned j=i+1;j<players.size();j++){
				restart|=((Sphere *)players[i]->getShape())->separateSpheres(players[j]->getShape());	
			}
		}
		count++;
		if (count==pow((double)players.size(),2)) cout<<"Could not separate spheres!!!"<<endl;
	}

	/*
	//Store old positions
	for (unsigned int i=0;i<players.size();i++){
		players[i]->getShape()->setOldPosition(players[i]->getShape()->getPosition());
	}
	*/
}

void World::detectGameOver()
{
	assert(!m_isOnVictoryScreen);
	int numAlivePlayers = 0;
	// Check for game-over state
	for (int i = 0; i < (int)players.size(); i++) {
		if (players[i]->getNumLives() > 0) {
			numAlivePlayers++;
			m_winnerIndex = i;
		}
	}
	if (numAlivePlayers <= 1) {
		m_isOnVictoryScreen = true;
		Sound::getInstance()->stopBackgroundMusic();
	}
}

bool World::isOnVictoryScreen()
{
	return m_isOnVictoryScreen;
}

int World::getWinnerIndex()
{
	return m_winnerIndex;
}

bool World::playerCollides(Player *player)
{
	for (unsigned i=0;i<players.size();i++){
		if (players[i]!=player){
			if (((Sphere *)players[i]->getShape())->collidesWith(player->getShape())) 
				return true;
		}
	}
	return false;
}

/*
void World::drawSpheres(int screenNum)
{
	int num_transparent = -1;	
	for (int i=0; i < players.size(); i++){
		if (players[i]->getNumLives()>0) {
			if ((players[i]->getTransparent()) && (screenNum==i)){
				num_transparent = i;
			}
			else players[i]->getShape()->drawMe(false);
		}
	}
	if (num_transparent!=-1) {
		players[num_transparent]->getShape()->drawMe(true);
	}

	for (int i=0; i < powerups.size(); i++){
		powerups[i]->drawMe();
	}

	drawShadows();
	glUniform1fARB(S_UNI_POS[S_UNI_lighting], 0.0);
	glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 1.0);
	glUniform1fARB(S_UNI_POS[S_UNI_shouldModulate], 1.0);
	glDepthMask(GL_FALSE);

	for(vector<ParticleEffects*>::iterator cur = particleEffects.begin(); cur != particleEffects.end(); ++cur) {
		(*cur)->drawMe();
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	for(vector<MissileParticleEffects*>::iterator it = missileParticleEffects.begin(); it != missileParticleEffects.end(); ++it) {
		(*it)->drawMe();
	}
	glDisable(GL_BLEND);

	glDepthMask(GL_TRUE);
	glUniform1fARB(S_UNI_POS[S_UNI_lighting], 1.0);
	glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 1.0);
	glUniform1fARB(S_UNI_POS[S_UNI_shouldModulate], 0.0);
}
*/

void World::rotatePowerupsBy(float angle, Vector3d axis)
{
	vector<powerupLocation>* vec;
	for (int i=0;i<powerups.size();i++){
		switch (powerups[i]->getType()) {
			case Powerup::MISSILE :
				vec = &missile_locations;
				break;
			case Powerup::BIG_BALL:
				vec = &bigball_locations;
				break;
			case Powerup::FAST_BALL:
				vec = &fastball_locations;
				break;
			default:
				assert(0);
				break;
		}
		for (int j=0;j<vec->size();j++)
		{
			if (powerups[i]->getInitialPosition() == (*vec)[j].position){
				(*vec)[j].rotation_angle = angle;
				(*vec)[j].rotation_axis = axis;
	
			}
		}
		if (!(powerups[i]->getType() == Powerup::MISSILE && ((Missile *)powerups[i])->hasLaunched())){
			powerups[i]->rotateBy(angle,axis);
		}
	}
}


void World::drawMe(int screenNum, bool missileSteer, bool isGameRunning)
{
	// render fully lit stage
	lighting();
	skyBox.drawMe();
	
	// If victory, draw winning player but no stage / powerups / smoke / shadows
	if (isOnVictoryScreen()) {
		players[m_winnerIndex]->getShape()->drawMe(false);
		drawShadows(); // For fireworks (cause drawShadows draws particles as well)

		glUniform1fARB(S_UNI_POS[S_UNI_lighting], 0.0);
		glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 1.0);
		glUniform1fARB(S_UNI_POS[S_UNI_shouldModulate], 1.0);
		glDepthMask(GL_FALSE);

		for(vector<ParticleEffects*>::iterator cur = particleEffects.begin(); cur != particleEffects.end(); ++cur) {
			(*cur)->drawMe();
		}

		glDepthMask(GL_TRUE);
		glUniform1fARB(S_UNI_POS[S_UNI_lighting], 1.0);
		glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 1.0);
		glUniform1fARB(S_UNI_POS[S_UNI_shouldModulate], 0.0);
	} else {
		if (stageDisplayList == 0 || stage->forceDraw()){
			stageDisplayList = glGenLists(1);
			glNewList(stageDisplayList, GL_COMPILE_AND_EXECUTE);
			stage->drawMe();
			stage->resetForceDraw();
			glEndList();
		}
		else {
			glCallList(stageDisplayList);
		}
		
		if (screenNum==-1){ //No split screen
			for (int i=0; i < players.size(); i++){
				if (players[i]->getNumLives()>0) {
					players[i]->getShape()->drawMe(false);
				}
			}
		}
		else { //SS --> check for transparent sphere
			int num_transparent = -1;	
			for (int i=0; i < players.size(); i++){
				if (players[i]->getNumLives()>0) {
					if ((players[i]->getTransparent()) && (screenNum==i) && (!missileSteer)){
						num_transparent = i;
					}
					else players[i]->getShape()->drawMe(false);
				}
			}
			if (num_transparent!=-1) {
				players[num_transparent]->getShape()->drawMe(true);
			}
		}

		for (int i=0; i < powerups.size(); i++){
			powerups[i]->drawMe();
		}

		drawShadows();
		

		glUniform1fARB(S_UNI_POS[S_UNI_lighting], 0.0);
		glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 1.0);
		glUniform1fARB(S_UNI_POS[S_UNI_shouldModulate], 1.0);
		glDepthMask(GL_FALSE);

		for(vector<ParticleEffects*>::iterator cur = particleEffects.begin(); cur != particleEffects.end(); ++cur) {
			(*cur)->drawMe();
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		for(vector<MissileParticleEffects*>::iterator it = missileParticleEffects.begin(); it != missileParticleEffects.end(); ++it) {
			(*it)->drawMe();
		}
		glDisable(GL_BLEND);

		glDepthMask(GL_TRUE);
		glUniform1fARB(S_UNI_POS[S_UNI_lighting], 1.0);
		glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 1.0);
		glUniform1fARB(S_UNI_POS[S_UNI_shouldModulate], 0.0);

	}
}


void World::drawShadows()
{
	// disable framebuffer writes, only going to stencil buffer here
	if (!drawShadowVolume) {
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	}
	glDepthMask(GL_FALSE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(0.0, 100.0);
	// also disable lighting and texturing
	glUniform1fARB(S_UNI_POS[S_UNI_lighting], 0.0);
	glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 0.0);

	// first pass, cull the front
	glCullFace(GL_FRONT);
	glStencilFunc(GL_ALWAYS, 0x0, 0xff);
	glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
	for (int i=0; i < players.size(); ++i){
		players[i]->drawShadowVolume();
	}
	for (int i=0; i < powerups.size(); i++){
		powerups[i]->drawShadowVolume();
	}
	// second pass, cull the back
	glCullFace(GL_BACK);
	glStencilFunc(GL_ALWAYS, 0x0, 0xff);
	glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
	for (int i=0; i < players.size(); ++i){
		players[i]->drawShadowVolume();
	}
	for (int i=0; i < powerups.size(); i++){
		powerups[i]->drawShadowVolume();
	}

	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_CULL_FACE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);

	glStencilFunc(GL_NOTEQUAL, 0x0, 0xff);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

	// turn on blending to blend the shadow
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	
	if (!isOnVictoryScreen()) {
		// draw a big dark quad and blend it in as the shadow
		drawShadow();
	}

	glDisable(GL_BLEND);
	glDisable(GL_STENCIL_TEST);

	// turn lighting and texturing back on
	glUniform1fARB(S_UNI_POS[S_UNI_lighting], 1.0);
	glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 1.0);

}

Player* World::getPlayer(unsigned int player_num)
{
	if (player_num>=players.size() || player_num<0) {
		return NULL;
	}	else {
		return players[player_num];
	}
}

unsigned int World::getNumPlayers()
{
	return players.size();
}

Stage* World::getStage()
{
	return stage;
}

void World::lighting() {
	GLfloat lmodel_ambient[] = {0.2, 0.2, 0.2, 1.0};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	glEnable(GL_LIGHT0);								// Enable Light 0
	
	glPushMatrix();
	GLfloat modelview[16];					// Where The 16 Doubles Of The Modelview Matrix Are To Be Stored
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);	

	static GLfloat LightAmb[] = {0.2f, 0.2f, 0.2f, 1.0f};	// Ambient Light
	static GLfloat LightDif[] = {1.0f, 1.0f, 1.0f, 1.0f};	// Diffuse Light
	static GLfloat LightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};	// Light
	Point light0Pos = getLightPos(0);
	GLfloat LightPos[] = {light0Pos[0], light0Pos[1], light0Pos[2], 1.0};
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmb);			// Set The Ambient Lighting For Light0
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDif);			// Set The Diffuse Lighting For Light0
	glLightfv(GL_LIGHT0, GL_POSITION, LightPos);		// Set The Position For Light0
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);		

	/**
	// need to multiply the light position by modelview matrix to pass in to the shader
	GLfloat lightPosMV[4];
	lightPosMV[0] = modelview[0]*LightPos[0] + modelview[4]*LightPos[1] + modelview[8]*LightPos[2] + modelview[12]*1.0;
	lightPosMV[1] = modelview[1]*LightPos[0] + modelview[5]*LightPos[1] + modelview[9]*LightPos[2] + modelview[13]*1.0;
	lightPosMV[2] = modelview[2]*LightPos[0] + modelview[6]*LightPos[1] + modelview[10]*LightPos[2] + modelview[14]*1.0;
	lightPosMV[3] = modelview[3]*LightPos[0] + modelview[7]*LightPos[1] + modelview[11]*LightPos[2] + modelview[15]*1.0;

	glUniform3fARB(S_UNI_POS[S_UNI_light0Pos], lightPosMV[0], lightPosMV[1], lightPosMV[2]);
	**/
	glPopMatrix();
}

unsigned int World::getNumLights() {
	return lightPositions.size();
}

Point World::getLightPos(int lightNum) {
	if (lightNum < lightPositions.size()) {
		return lightPositions[lightNum];
	} else {
		return Point();
	}
}

void World::drawShadow() {
	// disable depth testing
	glDisable(GL_DEPTH_TEST);

	// load the identity onto the modelview matrix
	glPushMatrix();
	glLoadIdentity();
	// load identity onto the projection matrix as well
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	// change projection to a nice unit ortho projection
	glOrtho(0, 1, 1, 0, 0, 1);

	glBegin(GL_QUADS);
	// set color for quad to be black at 50% alpha
	glColor4f(0.0, 0.0, 0.0, 0.5);
	glVertex2f(0.0, 0.0);
	glVertex2f(0.0, 1.0);
	glVertex2f(1.0, 1.0);
	glVertex2f(1.0, 0.0);
	glEnd();

	// cleanup
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
}


void World::steerMissileAttempt(Player* player, Vector3d force)
{
	for (int i=0;i<powerups.size();i++){
		if (powerups[i]->getType()==Powerup::MISSILE){
			((Missile *)powerups[i])->steer(player,force);
		}
	}
	
}


void World::launchMissileAttempt(Player* player, Vector3d direction)
{
	for (int i=0;i<powerups.size();i++){
		if (powerups[i]->getType()==Powerup::MISSILE){
			if (!playerLaunchedMissile(player)){
				((Missile *)powerups[i])->launch(player,direction);
			}
		}
	}
}


bool World::playerLaunchedMissile(Player *player, Vector3d* pos, Vector3d* vel)
{
	for (int i=0;i<powerups.size();i++){
		if (powerups[i]->getType()==Powerup::MISSILE){
			if (((Missile *)powerups[i])->playerLaunched(player)){
				if (pos!=NULL){
					*pos = powerups[i]->getPosition();
					*vel = ((Missile*)powerups[i])->getVelocity();
				}
				return true;
			}
		}
	}
	return false;
}

void World::addParticleEffect(ParticleEffects* particleEffect) {
	particleEffects.push_back(particleEffect);
}

void World::addMissileParticleEffect(MissileParticleEffects* missileParticleEffect) {
	missileParticleEffects.push_back(missileParticleEffect);
}

void World::toggleShadowVolume() {
	drawShadowVolume = !drawShadowVolume;
}