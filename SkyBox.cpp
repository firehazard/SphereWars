#include "SkyBox.h"
#include "Utils.h"
#include "SDLEngine.h"

SkyBox::SkyBox(void)
{
	displayListId = -1;
	textureId = 0;
	xRepetitions = 10.0;
	yRepetitions = 10.0;
	multipleTextures = false;
}

SkyBox::~SkyBox(void) {
	if (multipleTextures) {
		
	} else {
		//glDeleteTextures(1, &textureId);
	}
}

void SkyBox::init() {
	textureId = Utils::loadTexture("resources/space2.jpg");
	createDisplayList();
}

void SkyBox::drawMe() {
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glUniform1fARB(S_UNI_POS[S_UNI_lighting], 0.0);

	glPushMatrix();

	glCallList(displayListId);

	glPopMatrix();

	glEnable(GL_LIGHTING);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glUniform1fARB(S_UNI_POS[S_UNI_lighting], 1.0);
}

void SkyBox::setTextureAndRepetitions(GLuint texNum, float x, float y) {
	textureId = texNum;
	xRepetitions = x;
	yRepetitions = y;
	multipleTextures = false;
	createDisplayList();
}

void SkyBox::setTextureAndRepetitions(GLuint texNum, float x, float y, int faceIndex) {
	// TODO: Fill this in
	/**
	cout <<  "SKY BOX: Setting face #" << faceIndex 
	     << " to be the texture " << texNum
	     << " (" << x << "," << y << ")" << endl;  
	**/
	this->textureIds[faceIndex] = texNum;
	this->textureRepetitions[faceIndex] = Point(x, y, 0);
}

void SkyBox::commitChanges() {
	multipleTextures = true;
	createDisplayList();
}

void SkyBox::createDisplayList() {
	double sideLength = 2500.0;
	if (displayListId != -1) {
		glDeleteLists(displayListId, 1);
	}
	displayListId = glGenLists(1);
	glNewList(displayListId, GL_COMPILE);

	// Material
	/**
	GLfloat matAmbientDiffuse[] = {0.5f, 0.5f, 0.5f};
	GLfloat matSpecular[] = {1.0f, 1.0f, 1.0f};
	GLfloat matShininess[] = {0.0};

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbientDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
	**/
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	if (!multipleTextures) {
		glBindTexture(GL_TEXTURE_2D, textureId);
	}

	float overlap = 0;
	GLdouble vertices[] = {
		(-sideLength/2)-overlap, (sideLength/2)+overlap, (sideLength/2)+overlap, // 0
		(sideLength/2)+overlap, (sideLength/2)+overlap, (sideLength/2)+overlap, // 1
		(sideLength/2)+overlap, (sideLength/2)+overlap, (-sideLength/2)-overlap, // 2
		(-sideLength/2)-overlap, (sideLength/2)+overlap, (-sideLength/2)-overlap, // 3
		(-sideLength/2)-overlap, (-sideLength/2)-overlap, (sideLength/2)+overlap, // 4
		(sideLength/2)+overlap, (-sideLength/2)-overlap, (sideLength/2)+overlap, // 5
		(sideLength/2)+overlap, (-sideLength/2)-overlap, (-sideLength/2)-overlap, // 6
		(-sideLength/2)-overlap, (-sideLength/2)-overlap, (-sideLength/2)-overlap // 7
	};

	GLdouble texCoords[] = {
		0, 0,
		xRepetitions, 0,
		xRepetitions, yRepetitions,
		0, yRepetitions
	};

	// indices all start at the bottom left of the polygon
	GLubyte topIndices[] = {3, 2, 1, 0};
	GLubyte bottomIndices[] = {4, 5, 6, 7};
	GLubyte frontIndices[] = {3, 7, 6, 2};
	GLubyte backIndices[] = {5, 4, 0, 1};
	GLubyte leftIndices[] = {4, 7, 3, 0};
	GLubyte rightIndices[] = {6, 5, 1, 2};

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_DOUBLE, 0, vertices);

	updateTextures(0, texCoords);
	glBegin(GL_QUADS);
	for (int i = 0; i < 4; ++i) {
		//cout << texCoords[i*2] << "," << texCoords[(i*2)+1] << endl;
		glTexCoord2dv(&texCoords[i * 2]);
		glArrayElement(leftIndices[i]);
	}
	glEnd();

	updateTextures(1, texCoords);
	glBegin(GL_QUADS);
	for (int i = 0; i < 4; ++i) {
		//cout << texCoords[i*2] << "," << texCoords[(i*2)+1] << endl;
		glTexCoord2dv(&texCoords[i * 2]);
		glArrayElement(frontIndices[i]);
	}
	glEnd();

	updateTextures(2, texCoords);
	glBegin(GL_QUADS);
	for (int i = 0; i < 4; ++i) {
		glTexCoord2dv(&texCoords[i * 2]);
		glArrayElement(rightIndices[i]);
	}
	glEnd();

	updateTextures(3, texCoords);
	glBegin(GL_QUADS);
	for (int i = 0; i < 4; ++i) {
		glTexCoord2dv(&texCoords[i * 2]);
		glArrayElement(backIndices[i]);
	}
	glEnd();
	
	updateTextures(4, texCoords);
	glBegin(GL_QUADS);
	for (int i = 0; i < 4; ++i) {
		glTexCoord2dv(&texCoords[i * 2]);
		glArrayElement(bottomIndices[i]);
	}
	glEnd();
	
	updateTextures(5, texCoords);
	glBegin(GL_QUADS);
	for (int i = 0; i < 4; ++i) {
		glTexCoord2dv(&texCoords[i * 2]);
		glArrayElement(topIndices[i]);
	}
	glEnd();

	glEndList();
}

void SkyBox::updateTextures(int index, GLdouble* texCoordsOut) {
	if (multipleTextures) {
		glBindTexture(GL_TEXTURE_2D, textureIds[index]);
		//cout << textureIds[index] << endl;
		*(texCoordsOut) = 0.001;
		*(texCoordsOut+1) = 0.001;
		*(texCoordsOut+2) = textureRepetitions[index].x();
		*(texCoordsOut+3) = 0.001;
		*(texCoordsOut+4) = textureRepetitions[index].x();
		*(texCoordsOut+5) = textureRepetitions[index].y();
		*(texCoordsOut+6) = 0.001;
		*(texCoordsOut+7) = textureRepetitions[index].y();
	}
}