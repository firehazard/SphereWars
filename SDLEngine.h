#ifndef SDLENGINE_H
#define SDLENGINE_H

#include "sdlheaders.h"
#include "World.h"
#include "Sound.h"
#include "HeadsUpDisplay.h"
#include <math/point.h>
#include "Utils.h"
#include "menu/MenuManager.h"
#include <vector>
using namespace std;

#define S_UNI_light0Pos					0
#define S_UNI_lighting					1
#define S_UNI_bumpmap						2
#define S_UNI_shouldTexture			3
#define S_UNI_tex0							4
#define S_UNI_tex1							5
#define S_UNI_tex2							6
#define S_UNI_shouldParticle    7
#define S_UNI_time							8
#define S_UNI_shouldModulate		9
#define S_UNI_isSphere       		10

#define S_ATT_vertexTangent 		1
#define S_ATT_vertexVelocity 		4
#define S_ATT_vertexStartTime 	5



extern GLint S_UNI_POS[11];
extern double tick_delta;
extern double now;

class SDLEngine
{
private:
	static SDLEngine* instance;
	void reset();
	void initViewPoint();
	void resetViewPoint();
	void initOpenGL();
	void initSDL();
	void initShaders();
	void applyPlayersForces();
	void applySSPlayersForces();
	void moveCamera();
	void rotateCamera(const Vector3d &axis, float amt);
	void handleEyeUpdate();
	
	double last_tick;
	double framesTick;
	int frames;
	void computeDelta();
	void drawWorld();
	void drawMenu();
	void applyParams(const GameParameters *params);
	void autoRotateCamera(const Vector3d &axis, float amt);
	void handleEvents(bool isGameRunning);
	bool isFullScreen;
	bool isPause;
	bool isSplitScreen;
	void setVideoMode();
	void applyKeyboardForce(int player_num, bool up, bool down, bool left, bool right);
	void applySSKeyboardForce(int player_num, bool up, bool down, bool left, bool right);
	void applyJoystickForce(int player_num, float xaxis, float yaxis);
	void applySSJoystickForce(int player_num, float xaxis, float yaxis);
	void handleKeyPresses(SDLKey key, bool pressed, bool isGameRunning);
	void handleJoystickButtons(int player_num, int button);
	void handleMouseClicks(short x, short y, bool isDown);
	void applyInputForce(int player_num, Vector3d force, bool missile_steering);
	void launchMissile(int player_num);
	void updateSplitCams();
	void activateTransparency(int player_num, bool pressed);
	bool atLeastOneTransparent();
	bool isPlayerSteer(int player_num);
	void prepareVictoryScreen();
	void initMenuLevel();
		
	void initJoysticks();
	void endGame();


	struct Keys {
		bool upArrow;
		bool downArrow;
		bool leftArrow;
		bool rightArrow;
		bool a;
		bool d;
		bool s;
		bool w;
		bool f;
		bool l_alt;
		bool r_alt;
		bool escape;
		int m_xRel;
		int m_yRel;
		int m_yZoom;
	};

	struct JoyStick {
		short xAxis;
		short yAxis;
		short numButtons;
		Uint8* buttons;
	};
	
	struct Mouse {
		bool buttonIsDown;
		bool buttonNeedsHandling;
		short clickX;
		short clickY;
		short hoverX;
		short hoverY;
	};

	JoyStick* joysticks;
	int num_joysticks;

	Keys keys;
	Mouse mouse;
	
	GLdouble eyeX, eyeY, eyeZ;
	GLdouble targetX, targetY, targetZ;
	SDL_Joystick* joy1;
	SDL_Joystick* joy2;
	Sound sound;
	MenuManager menuMngr;
	HeadsUpDisplay hud;
	double lastShaderUpdateTime;
	double victoryStartTime;

	Vector3d camera;
	Vector3d horizontal;
	Vector3d* missile_vectors;

	struct splitCam {
		Vector3d from;
		Vector3d to;
		float view_angle;
	};
	splitCam* split_cameras;
	bool isVictoryScreenReady;
	FontManager::CachedTexture victoryMessage;
	bool returnToMenu;	
	unsigned int current_screen;


#define STEER_BUTTON 0
#define FIRE_BUTTON  1
#define CAM_BUTTON   2
#define PAUSE_BUTTON 9

public:
	static GLuint* textures;
	double extra_ratio;
	double extra_fov;
	GLhandleARB* shaderProgram;
	
	SDLEngine();
	~SDLEngine();
	static SDLEngine* getInstance();
	
	// Returns true when menu is over
	bool runMenu();
	
	// Returns true when game is over
	bool run();
	
	void init();
	Point getCameraLocation();

	int getDistanceFromCam(Vector3d location);

	
	static vector<Appearance> getSphereAppearances();
	static void quit();
	Vector3d getCameraDirection();
};


#endif