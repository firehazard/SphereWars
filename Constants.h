#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <climits>

#define INVALID_TEXTURE UINT_MAX


#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 768
//#define SCREEN_WIDTH  1900
//#define SCREEN_HEIGHT 1200
#define MIN_NUM_PLAYERS 2
#define MAX_NUM_PLAYERS 4

#define PI 3.14159265358979323846f
#define DEGREES_IN_A_CIRCLE 360.0f

#define GRAVITY -50.0f
#define WINNER_VIEWING_DISTANCE 15

//LOD constants
#define MAX_DIST_MDETAIL  300
#define MIN_DIST_FDETAIL  25
#define MAX_SUBDIVISIONS 30
#define MIN_SUBDIVISIONS  1
#define LEVELS_OF_DETAIL 10 //how many display lists 


//Collision
#define BOUNCINESS 1.5
#define VERTICAL_BOUNCINESS 1.2

//cam
#define Z_NEAR  1.0
#define Z_FAR   3000.0
#define FIELD_OF_VIEW 90.0

//Missile shooting, how far to push back
#define SHOT_BACK 300

//Input Force
#define INPUT_FORCE_SCALING 90

#endif /*CONSTANTS_H_*/
