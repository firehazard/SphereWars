#ifndef MATERIALPROPERTIES_H_
#define MATERIALPROPERTIES_H_

#include "sdlheaders.h"
#include "MaterialProperties.h"

struct MaterialProperties {
	bool isValid; // Indicates whether these MaterialProperties should be used. 
				  // (if not, some default set should be used.)
	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];
	GLfloat emission[4];
	GLfloat shininess;
};
	
#endif /*MATERIALPROPERTIES_H_*/
