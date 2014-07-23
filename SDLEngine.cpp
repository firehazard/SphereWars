#include "SDLEngine.h"
#include "Vector3d.h"
#include "Utils.h"
#include "Constants.h"
#include "Sound.h"
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <assert.h>
#include "FireworksParticleEffects.h"
#include "PowerupParticleEffects.h"
using namespace std;


double tick_delta;
double now;
GLint S_UNI_POS[11];
World* theWorld;

#define TICK_THRESHOLD 0.1
#define MENU_BACKGROUND_FILE "levels/menu_background.txt"


#define CAMERA_ROTATE_SPEED_MENU      5.0f
#define CAMERA_ROTATE_SPEED_VICTORY   10.0f

SDLEngine::SDLEngine()
 : hud(SCREEN_WIDTH, SCREEN_HEIGHT)
{
	instance = this;
	last_tick = Utils::getTime();
	framesTick = last_tick;
	frames = 0;
	
	tick_delta = 0;
	last_tick = Utils::getTime();
	framesTick = last_tick;
	tick_delta = 0;
	isFullScreen = false;
	theWorld = new World();	
	current_screen = -1;
	extra_fov = 0;
	extra_ratio = 0.80;

	shaderProgram = NULL;
	reset();	
}

void SDLEngine::reset()
{
	// quickly clear the keys struct
	memset(&keys, 0, sizeof(Keys));
	memset(&mouse, 0, sizeof(Mouse));

	isSplitScreen = false;
	isPause = false;
	resetViewPoint(); // Inits eyeX,Y,Z
	targetX = 0.0;
	targetY = 0.0;
	targetZ = 0.0;
	hud.setNumPlayers(0);
	hud.clearVictoryMessage();
	camera =  Vector3d(0 - eyeX, 0, 0 - eyeZ);
	Vector3d cameraVertical(camera.getX(), 1.0, camera.getZ());
	cameraVertical.normalize();
	horizontal = camera.crossProduct(cameraVertical);
	returnToMenu = false;
	
	theWorld->reset();
	menuMngr.init();
	
	Sound::getInstance()->loopBackgroundMusic("resources/cool.mid");
	isVictoryScreenReady = false;
}

SDLEngine::~SDLEngine()
{
	glDeleteProgram(*shaderProgram);
	free(shaderProgram);
	SDL_Quit();
}

SDLEngine* SDLEngine::instance;

SDLEngine* SDLEngine::getInstance() {
	return instance;
}

void SDLEngine::initMenuLevel()
{
	LevelInfo lvlInfo;
	lvlInfo.loadFromFile(MENU_BACKGROUND_FILE);
	theWorld->applyLevelInfo(&lvlInfo);
}


void SDLEngine::initViewPoint()
{
	eyeX = 0.0;
	eyeY = 50.0;
	eyeZ = 60.0;
}	

void SDLEngine::resetViewPoint()
{
	initViewPoint();
	handleEyeUpdate();	
}	


void SDLEngine::init() {
	//srand(Utils::getTime());
	initSDL();




	initShaders();
	lastShaderUpdateTime = Utils::getTime();
	theWorld->init();
	menuMngr.init();
	sound.loopBackgroundMusic("resources/cool.mid");
	initMenuLevel();
}

vector<Appearance> SDLEngine::getSphereAppearances()
{
	vector<Appearance> appearanceVec;
	GLfloat ambientDiffuse[] = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat emission[] = {0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat shininess = 100.0;
	Appearance appearance;
	appearance.bumpMap = false;
	memcpy(appearance.matProps.ambient, ambientDiffuse, sizeof(ambientDiffuse));
	memcpy(appearance.matProps.diffuse, ambientDiffuse, sizeof(ambientDiffuse));
	memcpy(appearance.matProps.emission, emission, sizeof(emission));
	memcpy(appearance.matProps.specular, specular, sizeof(specular));
	appearance.matProps.shininess = shininess;
	appearance.matProps.isValid = true;
	appearance.texNum = (textures[1]);

	appearanceVec.push_back(appearance);

	appearance.texNum = (textures[2]);
	appearanceVec.push_back(appearance);
	appearance.texNum = (textures[3]);
	appearanceVec.push_back(appearance);
	appearance.texNum = (textures[4]);
	appearanceVec.push_back(appearance);
	appearance.texNum = (textures[11]);
	appearanceVec.push_back(appearance);
	
	appearance.bumpMap = true;
	appearance.texNum = (textures[7]);
	appearance.tex2Num = (textures[8]);
	appearanceVec.push_back(appearance);

	appearance.texNum = (textures[9]);
	appearance.tex2Num = (textures[10]);
	appearanceVec.push_back(appearance);

	appearance.texNum = (textures[12]);
	appearance.tex2Num = (textures[13]);
	appearanceVec.push_back(appearance);

	return appearanceVec;
}

GLuint* SDLEngine::textures = new GLuint[15];

void SDLEngine::initOpenGL()
{
	textures[0] = Utils::loadTexture("resources/diamond_plate.jpg");
	textures[1] = Utils::loadTexture("resources/ball4.jpg");
	textures[2] = Utils::loadTexture("resources/stage1.bmp");
	textures[3] = Utils::loadTexture("resources/ball3.jpg");
	textures[4] = Utils::loadTexture("resources/ball.bmp");
	textures[5] = Utils::loadTexture("resources/diamond_plate_normal.jpg");
	textures[6] = Utils::loadTexture("resources/particle.bmp");
	textures[7] = Utils::loadTexture("resources/orange.jpg");
	textures[8] = Utils::loadTexture("resources/orange_heightmap.jpg");
	textures[9] = Utils::loadTexture("resources/earth.png");
	textures[10] = Utils::loadTexture("resources/earth_normal.png");
	textures[11] = Utils::loadTexture("resources/moon.png");
	textures[12] = Utils::loadTexture("resources/mars.png");
	textures[13] = Utils::loadTexture("resources/mars_normal.png");
	textures[14] = Utils::loadTexture("resources/grass.jpg");




	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glClearStencil(0);									// Clear The Stencil Buffer To 0
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glEnable(GL_TEXTURE_2D);							// Enable 2D Texture Mapping
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	
	glEnable(GL_LIGHTING);								// Enable Lighting

	//glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);	// Set Up Sphere Mapping
	//glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);	// Set Up Sphere Mapping
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glewInit();
	if (!(GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)) {
		printf("Failure loading extensions for GLSL\n");
	}
	
}


void SDLEngine::initSDL()
{
	atexit(SDL_Quit); // TODO: Not sure this does anything useful.
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 ) {
			printf("Unable to initialize: %s\n", SDL_GetError());
      exit(0);
    }	
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );
	setVideoMode();
	initJoysticks();
	initOpenGL();
	sound.init();
}

void SDLEngine::initJoysticks()
{
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	num_joysticks = SDL_NumJoysticks();
	joysticks = new JoyStick[num_joysticks];
	for (int i=0;i<num_joysticks;i++){
		joysticks[i].xAxis = 0;
		joysticks[i].yAxis = 0;
		joysticks[i].numButtons = SDL_JoystickNumButtons(SDL_JoystickOpen(i));
		joysticks[i].buttons = new Uint8[joysticks[i].numButtons];
		for (int j=0;j<joysticks[i].numButtons;j++){
			joysticks[i].buttons[j]=0;
		}
	}
}


void SDLEngine::initShaders() {
	char *vertexShaderSource;
	char *fragmentShaderSource;

	GLhandleARB vertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	GLhandleARB fragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);	
	

	vertexShaderSource = Utils::readTextFile("shaders/main.vert");
	fragmentShaderSource = Utils::readTextFile("shaders/main.frag");

	const char * vConst = vertexShaderSource;
	const char * fConst = fragmentShaderSource;

	//printf("%s\n", vConst);
	//printf("%s\n", fConst);
	glShaderSourceARB(vertexShader, 1, &vConst, NULL);
	glShaderSourceARB(fragmentShader, 1, &fConst ,NULL);

	glCompileShaderARB(vertexShader);
	Utils::printShaderInfoLog(vertexShader);
	glCompileShaderARB(fragmentShader);
	Utils::printShaderInfoLog(fragmentShader);

	shaderProgram = (GLhandleARB*)malloc(sizeof(GLhandleARB));
	*shaderProgram = glCreateProgramObjectARB();

	// reserve position locations for our variables
	glBindAttribLocation(*shaderProgram, S_ATT_vertexTangent, "vertexTangent");
	glBindAttribLocation(*shaderProgram, S_ATT_vertexVelocity, "vertexVelocity");
	glBindAttribLocation(*shaderProgram, S_ATT_vertexStartTime, "vertexStartTime");

	glAttachObjectARB(*shaderProgram, vertexShader);
	glAttachObjectARB(*shaderProgram, fragmentShader);

	glLinkProgramARB(*shaderProgram);
	Utils::printShaderInfoLog(*shaderProgram);
	glUseProgramObjectARB(*shaderProgram);

	free(vertexShaderSource);
	free(fragmentShaderSource);

	// TODO code to delete the programs since they are no longer needed
	S_UNI_POS[S_UNI_tex0] = glGetUniformLocationARB(*shaderProgram, "tex0");
	S_UNI_POS[S_UNI_tex1] = glGetUniformLocationARB(*shaderProgram, "tex1");
	S_UNI_POS[S_UNI_tex2] = glGetUniformLocationARB(*shaderProgram, "tex2");
	S_UNI_POS[S_UNI_light0Pos] = glGetUniformLocationARB(*shaderProgram, "light0Pos");
	S_UNI_POS[S_UNI_lighting] = glGetUniformLocationARB(*shaderProgram, "lighting");
	S_UNI_POS[S_UNI_shouldTexture] = glGetUniformLocationARB(*shaderProgram, "shouldTexture");
	S_UNI_POS[S_UNI_bumpmap] = glGetUniformLocationARB(*shaderProgram, "bumpmap");
	S_UNI_POS[S_UNI_shouldParticle] = glGetUniformLocationARB(*shaderProgram, "shouldParticle");
	S_UNI_POS[S_UNI_time] = glGetUniformLocation(*shaderProgram, "time");
	S_UNI_POS[S_UNI_shouldModulate] = glGetUniformLocation(*shaderProgram, "shouldModulate");
	S_UNI_POS[S_UNI_isSphere] = glGetUniformLocation(*shaderProgram, "isSphere");
	
	
	// default to lighting on
	glUniform1fARB(S_UNI_POS[S_UNI_lighting], 1.0);
	// default to texturing on
	glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 1.0);
	// default to disable bumpmap
	glUniform1fARB(S_UNI_POS[S_UNI_bumpmap], 0.0);
		// default to disable modulate
	glUniform1fARB(S_UNI_POS[S_UNI_shouldModulate], 0.0);
	glUniform1fARB(S_UNI_POS[S_UNI_isSphere], 0.0);
	
	//printf("%i %i %i\n", glGetUniformLocationARB(program, "light0Pos"), glGetUniformLocationARB(program, "light1Pos"), glGetUniformLocationARB(program, "light2Pos"));
}


void SDLEngine::setVideoMode()
{
	unsigned int flags = SDL_OPENGL;
	if (isFullScreen) {
		flags |= SDL_FULLSCREEN;
	}
	SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, flags);
}

void SDLEngine::applyPlayersForces()				
{
	applyKeyboardForce(0, keys.upArrow, keys.downArrow, keys.leftArrow, keys.rightArrow);
	applyKeyboardForce(1, keys.w, keys.s, keys.a, keys.d);
	
	for (int i=0;i<num_joysticks && i<theWorld->getNumPlayers();i++){
		applyJoystickForce(i,joysticks[i].xAxis,joysticks[i].yAxis);
	}
}


void SDLEngine::applySSPlayersForces()
{
	applySSKeyboardForce(0, keys.upArrow, keys.downArrow, keys.leftArrow, keys.rightArrow);
	applySSKeyboardForce(1, keys.w, keys.s, keys.a, keys.d);
	
	for (int i=0;i<num_joysticks  && i<theWorld->getNumPlayers();i++){
		applySSJoystickForce(i,joysticks[i].xAxis,joysticks[i].yAxis);
	}

	for (int i=0;i<theWorld->getNumPlayers(); i++) {
		missile_vectors[i] = split_cameras[i].to - split_cameras[i].from;
		missile_vectors[i].setY(0);
	}

}

void SDLEngine::applyKeyboardForce(int player_num, bool up, bool down, bool left, bool right)
{

	Vector3d force(0,0,0);
	double scaledTicDelta = tick_delta*INPUT_FORCE_SCALING;
	if (left) {
		force = force - (horizontal.getCopyWithMagnitude(scaledTicDelta));
	}
	if (right) {
		force = force + (horizontal.getCopyWithMagnitude(scaledTicDelta));
	}		
	if (up) {
		force = force + (camera.getCopyWithMagnitude(scaledTicDelta));	
	}
	if (down) {
		force = force - (camera.getCopyWithMagnitude(scaledTicDelta));	
	}

	if (force.getMagnitude()>0.5) missile_vectors[player_num] = force;
	
	
	if (!force.isZero()) {
		applyInputForce(player_num,force,isPlayerSteer(player_num));
	}

}


bool SDLEngine::isPlayerSteer(int player_num)
{
	return (((player_num==0 && keys.r_alt) || (player_num==1 && keys.l_alt) ) ||
		    (player_num<num_joysticks && joysticks[player_num].buttons[STEER_BUTTON]));

}


void SDLEngine::applySSKeyboardForce(int player_num, bool up, bool down, bool left, bool right)
{
	Vector3d force(0,0,0);
	Vector3d vel;
	Vector3d mpos;

	double scaledTicDelta = tick_delta*INPUT_FORCE_SCALING;
	bool inside = theWorld->getPlayer(player_num)->getTransparent();
	bool m_steer = isPlayerSteer(player_num) && !inside;
	bool inner_steer = isPlayerSteer(player_num)&& (theWorld->playerLaunchedMissile(theWorld->getPlayer(player_num), &mpos, &vel)) && inside;
	Vector3d normal(0,0,0);

	if (inner_steer){
			normal = vel.crossProduct(Vector3d::yAxis);
	}
	else if (m_steer){
			Vector3d diff = (split_cameras[player_num].to - split_cameras[player_num].from);
			normal = diff.crossProduct(Vector3d::yAxis);
	}
	if (!normal.isZero()){
			normal.normalize();
			normal = normal * scaledTicDelta;
	}

	Vector3d dir_force = split_cameras[player_num].from - split_cameras[player_num].to;
	dir_force.setY(0);
	if (!dir_force.isZero()) 
		dir_force = dir_force.getCopyWithMagnitude(scaledTicDelta);


	if (right) {
		if (m_steer || inner_steer) {
			force = force+normal;
		}
		else {
			split_cameras[player_num].view_angle+=scaledTicDelta;
			if (split_cameras[player_num].view_angle>360) split_cameras[player_num].view_angle-=360;
		}
	}
	if (left) {
		if (m_steer || inner_steer){
			force = force-normal;
		}
		else {
			split_cameras[player_num].view_angle-=scaledTicDelta;
			if (split_cameras[player_num].view_angle<0) split_cameras[player_num].view_angle+=360;
		}
	}

	if (up && !inner_steer) {
		force = force - dir_force;
	}
	if (down && !inner_steer) {
		force = force + dir_force;
	}
	
	if (!force.isZero()) {
		applyInputForce(player_num,force,m_steer || inner_steer);
	}
}


void SDLEngine::applyInputForce(int player_num, Vector3d force, bool missile_steering)
{
	if (missile_steering){
			theWorld->steerMissileAttempt(theWorld->getPlayer(player_num), force*0.5);
	}
	else {
		if (theWorld->getPlayer(player_num)->getShape()->isContactingStage()) {
			if (theWorld->getPlayer(player_num)->hasFastPowerup()) force = force * 2;
			theWorld->getPlayer(player_num)->getShape()->applyForceAdjustingForStage(force);
		}
	}
}

void SDLEngine::applyJoystickForce(int player_num, float xaxis, float yaxis)
{
	Vector3d force(0,0,0);
	double scaledTicDelta = tick_delta*INPUT_FORCE_SCALING;

	float val = xaxis / ((float)SHRT_MAX);
	
	force = force + horizontal.getCopyWithMagnitude(val * scaledTicDelta);

	val = yaxis / ((float)SHRT_MAX);
	force = force + camera.getCopyWithMagnitude(-1 * val * scaledTicDelta);
	
	if (force.getMagnitude()>0.5) missile_vectors[player_num] = force;

	if (!force.isZero()) {
		applyInputForce(player_num,force,isPlayerSteer(player_num));
	}
}


void SDLEngine::applySSJoystickForce(int player_num, float xaxis, float yaxis)
{
	Vector3d force(0,0,0);
	double scaledTicDelta = tick_delta*INPUT_FORCE_SCALING;
	
	Vector3d vel;
	Vector3d mpos;
	bool inside = theWorld->getPlayer(player_num)->getTransparent() && isPlayerSteer(player_num);
	bool m_steer = isPlayerSteer(player_num) && !inside;
	bool inner_steer = isPlayerSteer(player_num) && (theWorld->playerLaunchedMissile(theWorld->getPlayer(player_num), &mpos, &vel)) && inside;
	
	float xval = xaxis / ((float)SHRT_MAX);
	float yval = yaxis / ((float)SHRT_MAX);
	Vector3d dir_force = split_cameras[player_num].from - split_cameras[player_num].to;
	dir_force.setY(0);
	if (!dir_force.isZero()) {
		dir_force = dir_force.getCopyWithMagnitude(yval * scaledTicDelta);
	}
	if (!m_steer && !inner_steer) {
		split_cameras[player_num].view_angle+=(scaledTicDelta*xval);
		force = force + dir_force;
	}
	if (m_steer){
		Vector3d diff = (split_cameras[player_num].to - split_cameras[player_num].from);
		Vector3d normal = diff.crossProduct(Vector3d::yAxis);
		if (!normal.isZero()){
			normal.normalize();
			normal = normal * (xval*scaledTicDelta);
		}
		force = force + normal;
		force = force + dir_force;
	}
	if (inner_steer){
		Vector3d normal = vel.crossProduct(Vector3d::yAxis);	
		if (!normal.isZero()){
			normal.normalize();
			normal = normal * (xval*scaledTicDelta);
		}
		force = force + normal;
	}
	if (!force.isZero()) {
		applyInputForce(player_num,force,m_steer || inner_steer);
	}
}

void SDLEngine::moveCamera()
{
	if (keys.m_xRel != 0) {
		double theta = ((double)(2 * PI * keys.m_xRel) / (double)800.0);
		double t_eyeX = eyeX * cos(theta) - eyeZ * sin(theta);
		double t_eyeZ = eyeX * sin(theta) + eyeZ * cos(theta);
		eyeX = t_eyeX;
		eyeZ = t_eyeZ;
	}
	
	if (keys.m_yRel != 0) {
		eyeY += keys.m_yRel;		
	}

	if (keys.m_yZoom != 0) {
		Vector eye(eyeX, eyeY, eyeZ);
		Vector eyeAddition(eye);
		eyeAddition.normalize();
		eyeAddition.scale(keys.m_yZoom);
		eye = eye + eyeAddition;
		eyeX = eye.x();
		eyeY = eye.y();
		eyeZ = eye.z();
	}

	handleEyeUpdate();
}

void SDLEngine::handleEyeUpdate()
{
	camera =  Vector3d(0 - eyeX, 0, 0 - eyeZ);
	camera.normalize();
	// calculate a vertical camera vector as well, for cross product usage
	Vector3d cameraVertical(camera.getX(), 1.0, camera.getZ());
	cameraVertical.normalize();

	// Cross product will be a horizontal vector
	horizontal = camera.crossProduct(cameraVertical); 
}


void SDLEngine::handleKeyPresses(SDLKey key, bool pressed, bool isGameRunning)
{
	switch (key) {
		case SDLK_0:
			if (pressed) {
				theWorld->toggleShadowVolume();
			}
			break;
		case SDLK_RIGHT:
			keys.rightArrow = pressed;
			break;
		case SDLK_LEFT:
			keys.leftArrow = pressed;
			break;
		case SDLK_UP:
			keys.upArrow = pressed;
			break;
		case SDLK_DOWN:
			keys.downArrow = pressed;
			break;
		case SDLK_RCTRL:
			if (pressed) launchMissile(0);
			if (pressed && theWorld->isOnVictoryScreen()) {
				endGame();
			}
			break;
		case SDLK_LCTRL:
			if (pressed) launchMissile(1);
			if (pressed && theWorld->isOnVictoryScreen()) {
				endGame();
			}
			break;
		case SDLK_LALT:
			keys.l_alt = pressed;
			if (pressed && theWorld->isOnVictoryScreen()) {
				endGame();
			}
			break;
		case SDLK_RALT:
			if (pressed && theWorld->isOnVictoryScreen()) {
				endGame();
			}
			keys.r_alt = pressed;
			break;
		case SDLK_a:
			keys.a = pressed;
			break;
		case SDLK_d:
			keys.d = pressed;
			break;
		case SDLK_s:
			keys.s = pressed;
			break;
		case SDLK_w:
			keys.w = pressed;
			break;
		case SDLK_f:
			// Still allow full-screen changes on victory screen
			if (pressed) {
				//isFullScreen = !isFullScreen;
				//setVideoMode();
			}
			break;
		case SDLK_p:
			// Disable pausing on victory screen
			if (pressed && !theWorld->isOnVictoryScreen()) {
				 isPause = !isPause;
				 theWorld->updateMusic(isPause);
			}
			break;
		case SDLK_m:
			// Disable switching to splitscreen on victory screen
			if (pressed && !theWorld->isOnVictoryScreen()) { 
				isSplitScreen = !isSplitScreen;
			}
			break;
		case SDLK_1:
			if (!theWorld->isOnVictoryScreen()) { 
				activateTransparency(0,pressed);
			}
			break;
		case SDLK_2:
			if (!theWorld->isOnVictoryScreen()) { 
				activateTransparency(1,pressed);
			}
			break;
		case SDLK_3:
			if (!theWorld->isOnVictoryScreen()) { 
				activateTransparency(2,pressed);
			}
			break;
		case SDLK_4:
			if (!theWorld->isOnVictoryScreen()) { 
				activateTransparency(3,pressed);
			}
			break;
		case SDLK_8:
			if (pressed) { 
				extra_ratio+=0.01;
			}
			break;
		case SDLK_9:
			if (pressed) { 
				extra_ratio-=0.01;
			}
			break;

		case SDLK_6:
			if (pressed) { 
				extra_fov+=1;
			}
			break;
		case SDLK_7:
			if (pressed) { 
				extra_fov-=1;
			}
			break;


		case SDLK_ESCAPE:
			if (pressed) {
				if (isGameRunning) {
					endGame();
				} else {
					quit();
				}
			}
			break;
		default:
			break;
	}
}


void SDLEngine::activateTransparency(int player_num, bool pressed)
{
	if (player_num>=theWorld->getNumPlayers() || !isSplitScreen || !pressed) return;
	theWorld->getPlayer(player_num)->toggleTransparent();
}

void SDLEngine::handleJoystickButtons(int player_num, int button)
{
	switch (button) {
		case STEER_BUTTON:	break;
		case FIRE_BUTTON: 
			launchMissile(player_num);
			joysticks[player_num].buttons[FIRE_BUTTON] = false;
			break;
		case CAM_BUTTON:
			activateTransparency(player_num,true);
			break;
		case PAUSE_BUTTON:
			if (!theWorld->isOnVictoryScreen()) {
				 isPause = !isPause;
			}
			theWorld->updateMusic(isPause);
			break;
		default: break;
	}
}



void SDLEngine::handleMouseClicks(short x, short y, bool isDown)
{
	if (isDown) {
		mouse.buttonNeedsHandling = true; // To be deactivated after press is handled
		mouse.clickX = x;
		mouse.clickY = y;
	}
	mouse.buttonIsDown = isDown;
}

void SDLEngine::launchMissile(int player_num)
{
	if (!missile_vectors[player_num].isZero()){
		theWorld->launchMissileAttempt(theWorld->getPlayer(player_num),missile_vectors[player_num]);
	}
}



void SDLEngine::handleEvents(bool isGameRunning) {
	// reset mouse relative motion
	keys.m_xRel = 0;
	keys.m_yRel = 0;
	keys.m_yZoom = 0;

  SDL_Event event;
  while( SDL_PollEvent( &event ) ) {
		switch( event.type ) {
			case SDL_KEYDOWN:
				if (isGameRunning) handleKeyPresses(event.key.keysym.sym,true, isGameRunning);
				break;
			case SDL_KEYUP:
				handleKeyPresses(event.key.keysym.sym,false, isGameRunning);
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == 1) { // Should this be 1?					
					handleMouseClicks(event.button.x, event.button.y, true);
				}
				break;
			case SDL_MOUSEMOTION:
				mouse.hoverX = event.motion.x;
				mouse.hoverY = event.motion.y;
				if (event.motion.state & SDL_BUTTON(1)) {
					keys.m_xRel += event.motion.xrel;
					keys.m_yRel += event.motion.yrel;
				}
				if (event.motion.state & SDL_BUTTON(3)) {
					keys.m_yZoom += event.motion.yrel;
				}
				break;
			case SDL_JOYAXISMOTION:
				if (event.jaxis.axis == 0) {
					joysticks[event.jaxis.which].xAxis = event.jaxis.value;
				} else {
					joysticks[event.jaxis.which].yAxis = event.jaxis.value;
				}
				break;
			case SDL_JOYBUTTONDOWN:
				joysticks[event.jbutton.which].buttons[event.jbutton.button]=true;
				if (isGameRunning) {
					if (theWorld->isOnVictoryScreen()) {
						endGame();
					} else {
						if (event.jbutton.which<theWorld->getNumPlayers()){
							handleJoystickButtons(event.jbutton.which, event.jbutton.button);
						}
					}
				}
				break;
			case SDL_JOYBUTTONUP:
				joysticks[event.jbutton.which].buttons[event.jbutton.button]=false;
				break;
			case SDL_QUIT:
				SDL_Quit( );
				exit(1);
				break;
			default:
				// do nothing
				break;			
        }

  }
	
	if (isGameRunning) {
		// We're in a game. (but the game might be over)
		if (!theWorld->isOnVictoryScreen()) {
			if (!isPause){
				if (!isSplitScreen) {
					applyPlayersForces();
				} else {
					applySSPlayersForces();
				}
			}
			if (!isPause) {
				theWorld->updateWorld();
			}
			if (!isSplitScreen) {
				moveCamera();
			} else {
				updateSplitCams();
			}
		} else {
			// We're on victory screen.
			rotateCamera(Vector3d::yAxis, tick_delta * CAMERA_ROTATE_SPEED_VICTORY);
			theWorld->updateWorld();					
		}
	} else {
		// We're in a menu.
		// Pass in keys, joystick, and mouse presses to menuManager
		if (mouse.buttonNeedsHandling) {
			menuMngr.handleMouseClick(mouse.clickX, mouse.clickY);
			mouse.buttonNeedsHandling = false;
		} 
		menuMngr.handleMouseHover(mouse.hoverX, mouse.hoverY);
		//menuMngr.handleKeyboardInput();  // TODO 
		//menuMngr.handleJoystickInput();  // TODO
	}
}

void SDLEngine::updateSplitCams()
{
	for (int i=0;i<theWorld->getNumPlayers();i++){
		Vector3d pos = theWorld->getPlayer(i)->getShape()->getPosition();
		Vector3d view = Vector3d(pos.getX(),pos.getY()+10,pos.getZ()+10);
		view.rotateBy(split_cameras[i].view_angle, Vector3d::yAxis, pos);
		split_cameras[i].from = view;
		split_cameras[i].to = pos;
	}
}

bool SDLEngine::atLeastOneTransparent()
{
	for (int i=0;i<theWorld->getNumPlayers();i++)	{
		if (theWorld->getPlayer(i)->getTransparent()) return true;
	}
	return false;
}

void SDLEngine::drawWorld()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); /* clear window and z-buffer */
	if (isSplitScreen) {
		assert(!theWorld->isOnVictoryScreen());
		Vector3d pos;
		bool fullDisplayList = !atLeastOneTransparent();
		GLuint worldDL;
		int num_players = theWorld->getNumPlayers();
		for (int i=0;i<num_players;i++){
			current_screen = i;
			pos = theWorld->getPlayer(i)->getShape()->getPosition();
			switch (i){
				case 0: if (num_players==2) {
							glViewport(0,SCREEN_HEIGHT/2,SCREEN_WIDTH,SCREEN_HEIGHT/2);
						}
						else {
							glViewport(0,SCREEN_HEIGHT/2,SCREEN_WIDTH/2,SCREEN_HEIGHT/2);
						}
						break;
				case 1: if (num_players==2) {
							glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT/2);
						}
						else {
							glViewport(SCREEN_WIDTH/2,SCREEN_HEIGHT/2,SCREEN_WIDTH/2,SCREEN_HEIGHT/2);
						}
						break;
				case 2: glViewport(0,0,SCREEN_WIDTH/2,SCREEN_HEIGHT/2); 
						break;
				case 3: glViewport(SCREEN_WIDTH/2,0,SCREEN_WIDTH/2,SCREEN_HEIGHT/2);
						break;
			}

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			if (num_players==2) {
				gluPerspective(FIELD_OF_VIEW + extra_fov, SCREEN_WIDTH/(SCREEN_HEIGHT/2), Z_NEAR, Z_FAR);
			}
			else {
				//cout<<"Using extra ratio: "<<extra_ratio<<endl;
				gluPerspective(FIELD_OF_VIEW + extra_fov, ((double)SCREEN_WIDTH/(double)SCREEN_HEIGHT) * extra_ratio, Z_NEAR, Z_FAR);
			}
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			bool inner_steer = false;
			if (!theWorld->getPlayer(i)->getTransparent()){
				gluLookAt(split_cameras[i].from.getX(),split_cameras[i].from.getY(),split_cameras[i].from.getZ(), 
					      split_cameras[i].to.getX(), split_cameras[i].to.getY(), split_cameras[i].to.getZ(), 0.0, 1.0, 0.0);
			}
			else {
				Vector3d diff = split_cameras[i].to - split_cameras[i].from;
				Vector3d mpos;
				Vector3d vel;
				if (theWorld->playerLaunchedMissile(theWorld->getPlayer(i), &mpos, &vel) && isPlayerSteer(i)) {
					inner_steer = true;
					Vector3d dest = mpos + (vel*10);
					gluLookAt(mpos.getX(),mpos.getY(),mpos.getZ(),
						dest.getX(), dest.getY(), dest.getZ(), 0.0, 1.0, 0.0);
				}
				else {
					gluLookAt(split_cameras[i].to.getX(),split_cameras[i].to.getY(),split_cameras[i].to.getZ(), 
						      split_cameras[i].to.getX() + diff.getX(),split_cameras[i].to.getY(), split_cameras[i].to.getZ() + diff.getZ(), 0.0, 1.0, 0.0);
				}
			}
			//increase FPS as players die--- woohoo!
			if (theWorld->getPlayer(i)->getNumLives()>0) {
				theWorld->drawMe(i, inner_steer, true);
			}
			hud.drawMeSplitScreen(i);
/*
			if (fullDisplayList) {
				if (i==0) {
					worldDL = glGenLists(1);
					glNewList(worldDL, GL_COMPILE_AND_EXECUTE);
					theWorld->drawMe();
					hud.drawMe();
					glEndList();
				}
				else {
					glCallList(worldDL);
				}
				
			}
			else {
				theWorld->drawMe(false);
				theWorld->drawSpheres(i);
				hud.drawMe();
			}
			*/
		}
	}
	else {
		current_screen = -1;
		glViewport(0,0,SCREEN_WIDTH,SCREEN_HEIGHT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//cout<<"Using extra ratio: "<<extra_ratio<<endl;
		gluPerspective(FIELD_OF_VIEW + extra_fov, ((double)SCREEN_WIDTH/(double)SCREEN_HEIGHT) * extra_ratio, Z_NEAR, Z_FAR);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		gluLookAt(eyeX, eyeY, eyeZ, targetX, targetY, targetZ, 0.0, 1.0, 0.0);
		theWorld->drawMe(-1,false,true);
		hud.drawMe(); // Draws player info if game is running; victory message if game is over
	}
	glFlush();
	SDL_GL_SwapBuffers( );
}


// Assumes that we have a world initialized
// (e.g. we can choose a random map to circle around during the menu)
void SDLEngine::drawMenu()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); /* clear window and z-buffer */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FIELD_OF_VIEW, SCREEN_WIDTH/SCREEN_HEIGHT, Z_NEAR, Z_FAR);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(eyeX, eyeY, eyeZ, targetX, targetY, targetZ, 0.0, 1.0, 0.0);
	theWorld->drawMe(-1,false,false);
	menuMngr.drawMe();
	glFlush();
	SDL_GL_SwapBuffers( );
	
}

void SDLEngine::rotateCamera(const Vector3d &axis, float amt)
{			
	double theta = ((double)(2 * PI * amt) / (double)800.0);
	double shiftedEyeX = eyeX - targetX;
	double shiftedEyeZ = eyeZ - targetZ;
		
	double t_eyeX = shiftedEyeX * cos(theta) - shiftedEyeZ * sin(theta);
	double t_eyeZ = shiftedEyeX * sin(theta) + shiftedEyeZ * cos(theta);
	eyeX = t_eyeX + targetX;
	eyeZ = t_eyeZ + targetZ;
	handleEyeUpdate();
}

void SDLEngine::applyParams(const GameParameters *params)
{
	theWorld->setNumPlayers(params->getNumPlayers());
	hud.setNumPlayers(params->getNumPlayers());
	theWorld->applyLevelInfo(params->getLevelInfo());
	
	// TODO: theWorld->setLevelInfo(params->getLevelInfo());	
	missile_vectors = new Vector3d[theWorld->getNumPlayers()];
	split_cameras = new splitCam[theWorld->getNumPlayers()];
	for (unsigned i=0;i<theWorld->getNumPlayers();i++){
		 missile_vectors[i] = Vector3d(0,0,0);
		 split_cameras[i].from = Vector3d(0,0,0);
		 split_cameras[i].to = Vector3d(0,0,0);
		 split_cameras[i].view_angle = 0;
		 theWorld->getPlayer(i)->setAppearance(params->getAppearanceForPlayer(i));
	}
	
	
}

#define SKIP_MENU 0
bool SDLEngine::runMenu() 
{
	if (SKIP_MENU){
		theWorld->setNumPlayers(4);
		hud.setNumPlayers(4);
		// DEBUG
		LevelInfo lvlInfo;
		lvlInfo.loadFromFile("levels/basic.txt");
		// END DEBUG
		theWorld->applyLevelInfo(&lvlInfo);
		
		missile_vectors = new Vector3d[theWorld->getNumPlayers()];
		split_cameras = new splitCam[theWorld->getNumPlayers()];
		for (unsigned i=0;i<theWorld->getNumPlayers();i++){
			 missile_vectors[i] = Vector3d(0,0,0);
			 split_cameras[i].from = Vector3d(0,0,0);
			 split_cameras[i].to = Vector3d(0,0,0);
			 split_cameras[i].view_angle = 0;
			 theWorld->getPlayer(i)->setAppearance(getSphereAppearances()[i]);
		}
		return true;
	}
	
	tick_delta = Utils::getTime() - last_tick;
	last_tick = Utils::getTime();
	/*
	if (tick_delta>TICK_THRESHOLD){
		cout<<"Tick took too long - skipping frame"<<endl;
		return false;
	}
	*/
	rotateCamera(Vector3d::yAxis, tick_delta * CAMERA_ROTATE_SPEED_MENU);
	drawMenu();
	handleEvents(false);
	if (menuMngr.isReadyForGame()) {
		applyParams(menuMngr.getParams());
		resetViewPoint();
		return true;
	}
	return false;
}


bool SDLEngine::run() 
{
	char buffer[20];
	now = Utils::getTime();
	if ((now - framesTick) > 1.0) {
		sprintf(buffer, "%d FPS", frames);
		SDL_WM_SetCaption(buffer,"");
		printf("%d FPS\n", frames);
		frames = 0;
		framesTick = now;
	}
	tick_delta = now - last_tick;
	last_tick = now;
	if (tick_delta>TICK_THRESHOLD){
		//cout<<"Tick took too long: "<<tick_delta<<" - skipping frame"<<endl;
		return false;
	}
	
	/**
	if ((now - lastShaderUpdateTime) > 10) {
		cout<< "RE-initializing shaders" << endl;
		initShaders();
		lastShaderUpdateTime = now;
	}
	**/
	
	handleEvents(true);
	if (!isVictoryScreenReady && theWorld->isOnVictoryScreen()) {
		isSplitScreen = false;
		prepareVictoryScreen();
		isVictoryScreenReady = true;
		victoryStartTime = now;
	}
		
	if(isSplitScreen) tick_delta/=theWorld->getNumPlayers();	
	drawWorld(); // Should behave differently depending on theWorld->isOnVictoryScreen()
	frames++;
	
	if (returnToMenu) {
		reset();
		initMenuLevel();
		return true;
	}
	return false;
}

void SDLEngine::endGame()
{
	// force 5 seconds in the victory screen
	if ((now - victoryStartTime) > 5.0) {
		// Disable split screen, if enabled
		isSplitScreen = false;
		returnToMenu = true;
	}
}

void SDLEngine::quit() 
{
	cout << "Quitting..." << endl;
	SDL_Quit();
	exit(0);
}

Point SDLEngine::getCameraLocation() {
	if (!isSplitScreen){
		return Point(eyeX, eyeY, eyeZ);
	}
	else {
		assert(current_screen!=-1);
		if (theWorld->getPlayer(current_screen)->getTransparent()){
			Vector3d mpos;
			Vector3d vel;
			if (theWorld->playerLaunchedMissile(theWorld->getPlayer(current_screen), &mpos, &vel) && isPlayerSteer(current_screen)){
				return Point(mpos.getX(), mpos.getY(), mpos.getZ());
			}
			else {
				return Point(split_cameras[current_screen].to.getX(), split_cameras[current_screen].to.getY(),
					split_cameras[current_screen].to.getZ());
			}
		}
		else {
			return Point(split_cameras[current_screen].from.getX(), split_cameras[current_screen].from.getY(),
				split_cameras[current_screen].from.getZ());
		}
	}
	
}

void SDLEngine::prepareVictoryScreen()
{
	int winningPlayerNum = theWorld->getWinnerIndex();
	assert(winningPlayerNum < theWorld->getNumPlayers());
	// TODO: Create a "Player X Wins!" banner
	// or a "It's a draw!" banner if there's no winner

	// Move winner to origin
	if (winningPlayerNum != NO_WINNER) {
		theWorld->getPlayer(winningPlayerNum)->getShape()->setPosition(Vector3d::zero);
		hud.prepareVictoryMessage(winningPlayerNum);
	}
	// Move camera to a set distance from origin, so we can rotate around it nicely
	eyeX = 0;
	eyeY = 0;
	eyeZ = WINNER_VIEWING_DISTANCE;
}

int SDLEngine::getDistanceFromCam(Vector3d location)
{
	Vector3d distance;
	if (!isSplitScreen) {
		distance = Vector3d(eyeX,eyeY,eyeZ) - location;
	}
	else {
		assert(current_screen!=-1);
		if (theWorld->getPlayer(current_screen)->getTransparent()){
			Vector3d mpos;
			Vector3d vel;
			if (theWorld->playerLaunchedMissile(theWorld->getPlayer(current_screen), &mpos, &vel) && isPlayerSteer(current_screen)){
				distance = mpos - location;
			}
			else {
				distance = split_cameras[current_screen].to - location;
			}
		}
		else {
			distance = split_cameras[current_screen].from - location;
		}
	}
	return abs((int)distance.getMagnitude());
}

Vector3d SDLEngine::getCameraDirection()
{
	assert(isSplitScreen);
	Vector3d location = theWorld->getPlayer(current_screen)->getShape()->getPosition();
	Vector3d direction = location - split_cameras[current_screen].from;
	direction.setY(0);
	return direction.getUnitVector();
}