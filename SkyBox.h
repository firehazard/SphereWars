#pragma once

#include "sdlheaders.h"
#include <math/point.h>

class SkyBox
{
	public:
		SkyBox(void);
		void init();
		void reset();
		void drawMe();
		virtual ~SkyBox(void);
		void setTextureAndRepetitions(GLuint texNum, float x, float y);
		void setTextureAndRepetitions(GLuint texNum, float x, float y, int faceIndex);
		void commitChanges();

	private:
		int displayListId;
		GLuint textureId;
		float xRepetitions;
		float yRepetitions;
		GLuint textureIds[6];
		Point textureRepetitions[6];
		bool multipleTextures;
		
		void createDisplayList();
		void updateTextures(int index, GLdouble* texCoordsOut);
};
