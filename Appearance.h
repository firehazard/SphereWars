#ifndef APPEARANCE_H_
#define APPEARANCE_H_

#include "sdlheaders.h"
#include "MaterialProperties.h"

struct Appearance {
	GLuint texNum;
	GLuint tex2Num;
	bool bumpMap;
	MaterialProperties matProps;
};


#endif /*APPEARANCE_H_*/
