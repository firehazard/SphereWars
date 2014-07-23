#ifndef UTILS_H
#define UTILS_H

#include "sdlheaders.h"
#include "SDL/SDL_image.h"
#include "Vector3d.h" // Remove after converting getAngleInRadians
#include <map>
#include <math.h>
#include <string>
using namespace std;

#ifdef WIN32
# include <sys/timeb.h>
#else
# include <sys/time.h>
#endif


class Utils
{
	private:
		Utils(void) {}
		~Utils(void) {}
		
		static int round(double x);
		
		static map<string, GLuint> s_textureCache;
		
	public:
		static SDL_Surface* loadBitmap(const string &fileName);
		static GLuint loadTexture(const string &fileName);
		static double radToDeg(double angleInRadians);
		static double degToRad(double angleInDegrees);
		static char* readTextFile(const char* fileName);
		static void printShaderInfoLog(GLhandleARB shader);
		static int nextPowerOfTwo(int x);
		static float getAngleInRadians(const Vector3d &a, const Vector3d &b,
                                       const Vector3d &c);
        static bool isAcuteAngle(const Vector3d &a, const Vector3d &b, 
                                    const Vector3d &c);
		static double randomSign();
		static double random();
		static double getTime();

};

typedef struct {
	float x;
	float y;
	float z;
} Vect3df;

void vectAdd(Vect3df* src1, Vect3df* src2, Vect3df* dest);
void vectSub(Vect3df* src1, Vect3df* src2, Vect3df* dest);
void vectCross(Vect3df* src1, Vect3df* src2, Vect3df* dest);
void vectDiv(Vect3df* src1, float divisor, Vect3df* dest);
void vectNorm(Vect3df* src);
void vectCopy(Vect3df* src, Vect3df* dest);
unsigned char maxuc(unsigned char* s1, unsigned char* s2, unsigned char* s3);

#endif
