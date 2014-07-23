#include "Utils.h"
#include "Constants.h"
#include "Vector3d.h"
#include <climits>
#include <math.h>
#include <iostream>
#include <stdio.h>
using namespace std;


map<string, GLuint> Utils::s_textureCache;

SDL_Surface* Utils::loadBitmap(const string &fileName) {
	SDL_Surface *image = 0;
	image = IMG_Load(fileName.c_str());
	
	return image;
}

/*
 * Returns UINT_MAX if there's a problem reading
 * in a texture from the file.
 */
GLuint Utils::loadTexture(const string &fileName) {
	// See if we've already loaded the texture
	map<string, GLuint>::iterator iter = s_textureCache.find(fileName);
	if (iter != s_textureCache.end()) {
		return iter->second;
	}
	
	SDL_Surface* s = Utils::loadBitmap(fileName);
	if (!s) {
		// TODO: How to handle this -- should we abort, or 
		// just use a plain color for the skin, or what?
		cerr << "Warning: Unable to load texture from file \"" 
		     << fileName << "\"" << endl;
		return UINT_MAX;
	}

	// texture is upside down we need to flip it
	uint8_t* pixels = (uint8_t*)s->pixels;
	uint8_t* newPixels = (uint8_t*)malloc(s->format->BytesPerPixel * s->w * s->h);

	for (int y = 0; y < s->h; ++y) {
		uint8_t* r1 = pixels;
		uint8_t* r2 = newPixels;
		r1 += (s->format->BytesPerPixel * s->w * y);
		r2 += (s->format->BytesPerPixel * s->w * (s->h - 1 - y));
		memcpy(r2, r1, s->format->BytesPerPixel * s->w);
	}
	s->pixels = newPixels;

	GLuint texture = 0;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	
	if (s->format->BitsPerPixel == 32) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
	} else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, s->w, s->h, 0, GL_RGB, GL_UNSIGNED_BYTE, s->pixels);
	}
	
	s->pixels = pixels;
	free(newPixels);

	SDL_FreeSurface(s);
	
	// Update cache
	s_textureCache[fileName] = texture;
	return texture;
}

double Utils::radToDeg(double angleInRadians)
{
	return angleInRadians * DEGREES_IN_A_CIRCLE / (2 * PI); 
}
double Utils::degToRad(double angleInDegrees)
{
	return angleInDegrees * (2 * PI) / DEGREES_IN_A_CIRCLE; 
}

char* Utils::readTextFile(const char* fileName) {
	FILE *file = NULL;
	char* content = NULL;
	file = fopen(fileName, "rt");
	if (file == NULL) {
		printf("Failure opening file: %s\n", fileName);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	fseek(file, 0, SEEK_SET);
	if (size > 0) {
		content = (char*)malloc(sizeof(char) * (size+1));
		memset(content, 0, size);
		int readSize = fread(content, 1, size, file);
		content[readSize] = '\0';
	}

	fclose(file);

	return content;
}

void Utils::printShaderInfoLog(GLhandleARB shader) {
	int length;
	glGetObjectParameterivARB(shader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &length);
	if (length > 0) {
		char* log = (char*)malloc(sizeof(char)* length);
		int written;
		glGetInfoLogARB(shader, length, &written, log);
		printf("%s\n", log);
		free(log);
	}
}

int Utils::nextPowerOfTwo(int x)
{
	double logbase2 = log((double)x) / log((double)2);
	return Utils::round(pow(2,ceil(logbase2)));
}

int Utils::round(double x)
{
	return (int)(x + 0.5);
}

// Returns the measure of angle ABC in radians
// TODO: Convert to Vect3df
float Utils::getAngleInRadians(const Vector3d &a, 
                               const Vector3d &b, 
                               const Vector3d &c)
{
	const Vector3d u = (a-b);
	const Vector3d v = (c-b);
	
	float dotProd = u.dotProduct(v);
	float scaledDotProd = dotProd/(u.getMagnitude() * v.getMagnitude());
	return acos(scaledDotProd);
}

bool Utils::isAcuteAngle(const Vector3d &a, 
                            const Vector3d &b, 
                            const Vector3d &c)
{
	float angle = getAngleInRadians(a,b,c);
	return (angle < PI / 2);
}

double Utils::randomSign() {
	double sign = (double)rand() / (double)RAND_MAX;
	if (sign < 0.5) {
		sign = -1.0;
	} else {
		sign = 1.0;
	}
	return sign;
}

double Utils::random() {
	return (double)((double)rand() / (double)RAND_MAX);
}

void vectAdd(Vect3df* src1, Vect3df* src2, Vect3df* dest) {
	dest->x = src1->x + src2->x;
	dest->y = src1->y + src2->y;
	dest->z = src1->z + src2->z;
}

void vectSub(Vect3df* src1, Vect3df* src2, Vect3df* dest) {
	dest->x = src1->x - src2->x;
	dest->y = src1->y - src2->y;
	dest->z = src1->z - src2->z;
}

void vectCross(Vect3df* src1, Vect3df* src2, Vect3df* dest) {
	dest->x = src1->y*src2->z - src1->z*src2->y;
	dest->y = src1->z*src2->x - src1->x*src2->z;
	dest->z = src1->x*src2->y - src1->y*src2->x;
}

void vectDiv(Vect3df* src1, float divisor, Vect3df* dest) {
	dest->x = src1->x / divisor;
	dest->y = src1->y / divisor;
	dest->z = src1->z / divisor;
}

void vectNorm(Vect3df* src) {
	float divisor = sqrtf(src->x * src->x + src->y * src->y + src->z * src->z);
	vectDiv(src, divisor, src);
}

void vectCopy(Vect3df* src, Vect3df* dest) {
	dest->x = src->x;
	dest->y = src->y;
	dest->z = src->z;
}

unsigned char maxuc(unsigned char* s1, unsigned char* s2, unsigned char* s3) {
	unsigned char max = *s1;
	if ((*s2) > max) {
		max = *s2;
	}
	if ((*s3) > max) {
		max = *s3;
	}
	return max;
}


double Utils::getTime() {
	#ifdef WIN32
	  struct _timeb tbuf;
	  _ftime64_s(&tbuf);
	  return ((double)tbuf.time + 0.001 * (double)(tbuf.millitm)); 
	#else
	  struct timeval time;
	  gettimeofday(&time, NULL);
	  return ((double)time.tv_sec + 0.000001 * (double)time.tv_usec); 
	#endif
}
