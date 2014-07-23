#ifndef POWERUP_H_
#define POWERUP_H_

class Player;
class Missile;

#include "Vector3d.h"
#include "sdlheaders.h"
#include "Shape.h"
#include <string>

class Powerup
{
public:
	typedef enum {
		MISSILE,
		BIG_BALL,
		FAST_BALL,
		NUM_POWERUP_TYPES
	} PowerupType;

	Powerup(const Vector3d &theInitialPosition);
	virtual ~Powerup(void);
	virtual void reset();
	
	Vector3d getPosition();
	virtual void setOwner(Player* player);
	virtual void drawMe();
	virtual void drawShadowVolume();
	virtual void updateState();
	virtual bool mustDestroy();
	PowerupType getType();
	Vector3d getInitialPosition();
	
	virtual void rotateBy(float angle, Vector3d axis);


	// Returns NUM_POWERUP_TYPES if there is no powerup with the given name
	static PowerupType   getTypeForName(const string &powerupName);
	static const string &getNameForType(PowerupType type);
	static Powerup *buildPowerupOfType(PowerupType type, const Vector3d &thePosition);

protected:
	struct gluObject {
		GLUquadricObj* glu_shape;		
		GLuint displayListId;
	};

	Player *owner;
	Vector3d initialPosition;
	Vector3d position;
	//GLUquadricObj* powerup_box;
	//GLuint powerbox_ID;
	PowerupType ptype;
	bool collidesWith(Shape *other_shape);
	int getDetailLevel(Vector3d pos);
	float powerup_color;
	bool  is_add_color;

private:

	gluObject powerup_boxes[LEVELS_OF_DETAIL];

	
	static const string s_powerupNames[];

	void initDisplayList();
};

#include "Player.h"

#endif
