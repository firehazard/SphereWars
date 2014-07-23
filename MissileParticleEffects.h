#ifndef MISSILEPARTICLEEFFECTS_H_
#define MISSILEPARTICLEEFFECTS_H_
#pragma once
#include "Missile.h"
#include "sdlheaders.h"
#include <glt/color.h>

class Missile;

class MissileParticleEffects
{
	public:
		MissileParticleEffects(Missile* missile);
		virtual ~MissileParticleEffects(void);
		void init();
		void reset();
		void updateState();
		void drawMe();
		void setExpired(bool expired);
		bool getExpired();

	private:
		bool isInitialized;
		Missile* myMissile;
		int maxParticles;
		int activeParticles;
		int curParticle;
		double initialTime;
		double lastTime;
		double minTick;
		float curRelativeTime;

		float particleLifeTime;
		GltColor* startColor;
		GltColor* endColor;
		GLfloat* vertexPointer;
		GLfloat* colorPointer;
		GLfloat* velocityPointer;
		GLfloat* startTimePointer;
		bool expired;

		void clear();
		void updateColors();	
		GLfloat** getPointers(int index);
};

#endif
