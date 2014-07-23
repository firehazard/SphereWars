#include "Stage.h"
#include "SDLEngine.h"
#include "Utils.h"

#define WIDTH    1.0
#define DEPTH    1.0
#define HEIGHT   1.0


Stage::Stage(void) {
	loadMeshFromImage("resources/stage1.bmp");
}

Stage::~Stage(void) {
}

void Stage::drawMe(void) {
	glPushMatrix();
	glLoadIdentity();
	glTranslated(-(width / 2), 0, -(height / 2));
	glCallList(stageDisplayList);
	glPopMatrix();
}

double Stage::getFrictionAt(const Vector3d &position) const
{
	return 0.005;
}

void Stage::loadMeshFromImage(char* imageName) {
	SDL_Surface* surface = Utils::loadBitmap(imageName);
	vertices = (GLfloat*)malloc(sizeof(GLfloat) * surface->w * surface->h * 3);
	width = surface->w;
	height = surface->h;
	
	// create vertices from the image
	int x, z;
	
	unsigned char* pixels = (unsigned char*)surface->pixels;
	unsigned char* red, *green, *blue, *alpha;
	GLfloat* vertPointer = vertices;
	
	for (z = 0; z < height; ++z) {
		for (x = 0; x < width; ++x) {
			blue = pixels++;
			green = pixels++;
			red = pixels++;
			*vertPointer = (GLfloat)x;
			//printf("X: %f ", *vertPointer);
			++vertPointer;
			// set the height equal to the brightness of the pixel
			*vertPointer = maxuc(red, green, blue);
			//printf("Y: %f ", *vertPointer);
			++vertPointer;
			*vertPointer = (GLfloat)z;
			//printf("Z: %f \n", *vertPointer);
			++vertPointer;		
		}
	}

	// compute normals for all the polygons
	polyNormals = (GLfloat*)malloc(sizeof(GLfloat) * surface->w * surface->h * 6);
	
	Vect3df *v1 = (Vect3df*)vertices;
	Vect3df *v2 = v1 + 1;
	Vect3df *v3 = (Vect3df*)(vertices + (width * 3));
	Vect3df *v4 = v3 + 1;

	Vect3df *normalsPtr = (Vect3df*)polyNormals;

	for (z = 0; z < (height-1); ++z) {
		for (x = 0; x < (width-1); ++x) {
			Vect3df l1,l2,l3;

			// because of triangle strips we have to change our calculations here
			if (!(x % 2)) {
				vectSub(v2, v1, &l1);
				vectSub(v2, v3, &l2);
				vectSub(v2, v4, &l3);
			} else {
				vectSub(v1, v3, &l1);
				vectSub(v1, v4, &l2);
				vectSub(v1, v2, &l3);
			}
			vectCross(&l1, &l2, normalsPtr);			

			vectNorm(normalsPtr);

			++normalsPtr;
			vectCross(&l2, &l3, normalsPtr);			
			
			vectNorm(normalsPtr);
			++normalsPtr;

			v1 = v2;
			++v2;
			v3 = v4;
			++v4;
		}
		v1 = v2;
		++v2;
		v3 = v4;
		++v4;
	}

	// average the polygon normals to compute the vertex normals
	vertexNormals = (GLfloat*)malloc(sizeof(GLfloat) * surface->w * surface->h * 3);

	// begin special case top row
	Vect3df* curNormal = (Vect3df*)vertexNormals;
	v1 = (Vect3df*)polyNormals;
	vectCopy(v1, curNormal);
	v2 = v1+1;
	v3 = v2+1;
	v4 = v3+1;
	++curNormal;

	for (x=1; x < (width-1); ++x) {
		if (x % 2) {
			vectAdd(v1, v2, curNormal);
			vectAdd(curNormal, v3, curNormal);
			vectAdd(curNormal, v4, curNormal);
			vectNorm(curNormal);
		} else {
			vectAdd(v2, v3, curNormal);
			vectNorm(curNormal);
		}

		v1 = v3;
		v2 = v4;
		v3 = v2+1;
		v4 = v3+1;
		++curNormal;
	}

	// average the last two for the last part of the top row
	vectAdd(v1, v2, curNormal);
	vectNorm(curNormal);
	++curNormal;
	v1 = v3;
	v2 = v4;
	v3 = v2+1;
	v4 = v3+1;

	// end special case top row

	Vect3df* v5 = v1 + ((width - 1) * 2);
	Vect3df* v6 = v5 + 1;
	Vect3df* v7 = v6 + 1;
	Vect3df* v8 = v7 + 1;

	// begin middle general case
	for (z = 1; z < (height - 1); ++z) {
		vectAdd(v1, v2, curNormal);
		vectAdd(curNormal, v3, curNormal);
		vectNorm(curNormal);
		++curNormal;

		for (x = 1; x < (width - 1); ++x) {
			if (x % 2) {
				vectAdd(v2, v3, curNormal);
				vectAdd(curNormal, v5, curNormal);
				vectAdd(curNormal, v6, curNormal);
				vectAdd(curNormal, v7, curNormal);
				vectAdd(curNormal, v8, curNormal);
			} else {
				vectAdd(v1, v2, curNormal);
				vectAdd(curNormal, v3, curNormal);
				vectAdd(curNormal, v4, curNormal);
				vectAdd(curNormal, v6, curNormal);
				vectAdd(curNormal, v7, curNormal);
			}
			vectNorm(curNormal);			
			v1 = v3;
			v2 = v4;
			v3 = v2+1;
			v4 = v3+1;
			v5 = v7;
			v6 = v8;
			v7 = v6+1;
			v8 = v7+1;
			++curNormal;
		}

		vectAdd(v2, v5, curNormal);
		vectAdd(curNormal, v6, curNormal);
		vectNorm(curNormal);
		v1 = v3;
		v2 = v4;
		v3 = v2+1;
		v4 = v3+1;
		v5 = v7;
		v6 = v8;
		v7 = v6+1;
		v8 = v7+1;
		++curNormal;
	}

	// begin bottom row special case
	vectAdd(v1, v2, curNormal);
	vectNorm(curNormal);
	v1 = v3;
	v2 = v4;
	v3 = v2+1;
	v4 = v3+1;

	for (x=1; x < (width-1); ++x) {
		if (x % 2) {
			vectAdd(v1, v2, curNormal);
			vectAdd(curNormal, v3, curNormal);
			vectAdd(curNormal, v4, curNormal);
			vectNorm(curNormal);
		} else {
			vectAdd(v2, v3, curNormal);
			vectNorm(curNormal);
		}

		v1 = v3;
		v2 = v4;
		v3 = v2+1;
		v4 = v3+1;
		++curNormal;
	}

	vectCopy(v2, curNormal);
	vectNorm(curNormal);
	// end bottom row special case

	// Begin Gen Display List
	stageDisplayList = glGenLists(1);
	GLuint blah = stageDisplayList;
	glNewList(stageDisplayList, GL_COMPILE);
	
	GLfloat* curRowVert = vertices;
	GLfloat* nextRowVert = vertices + (width * 3);
	GLfloat* curRowNorm = vertexNormals;
	GLfloat* nextRowNorm = vertexNormals + (width*3);

	for (z = 0; z < (height-1); ++z) {
		glBegin(GL_TRIANGLE_STRIP);
		for (x = 0; x < width; ++x) {
			//printf("%f %f %f\n", *currentRow, *(currentRow+1), *(currentRow+2));
			//printf("%f %f %f\n", *curRowNorm, *(curRowNorm+1), *(curRowNorm+2));
			glNormal3fv(curRowNorm);			
			glVertex3fv(curRowVert);
			//printf("%f %f %f\n", *nextRow, *(nextRow+1), *(nextRow+2));
			glNormal3fv(nextRowNorm);
			glVertex3fv(nextRowVert);

			curRowVert += 3;
			nextRowVert += 3;
			curRowNorm += 3;
			nextRowNorm += 3;			
		}
		glEnd();
	}
	
	glEndList();
	// End Gen Display List
}
