#ifndef STAGE_H
#define STAGE_H

#include "sdlheaders.h"
#include "Vector3d.h"
#include "Utils.h"

class Stage
{
  public:
	Stage(void);
	virtual ~Stage(void);

	void drawMe();
	double getFrictionAt(const Vector3d &position) const;

	private:
		// Variables
		GLfloat* vertices;
		GLfloat* polyNormals;
		GLfloat* vertexNormals;
		GLint width;
		GLint height;
		GLuint stageDisplayList;

		// Functions
		void loadMeshFromImage(char* imageName);
};

#endif
