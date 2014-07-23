#ifndef PARTICLEEFFECTS_H
#define PARTICLEEFFECTS_H
#pragma once

#include "sdlheaders.h"
#include <glt/color.h>
#include <math/vector3.h>
#include <math/point.h>


struct Particle {
	bool			alive;
	Vector		pos; // vector instead of point for less storage space
	Vector		velocity;
	Vector		size;
	GltColor	color;
	float			time;
	Vector		v1;
	/**
	Vector		v2;
	Vector		v3;
	**/
	float			f1;			
	/**
	float			f2;			
	float			f3;			
	**/
};

class ParticleEffects
{
	public:
		ParticleEffects(int maxParticles, int particlesPerTick, Vector initialPosition,
			float particleLifeTime, int textureNumber, Vector initialVelocity, float lifeTime);
		virtual ~ParticleEffects(void);
		virtual inline void updateState();
		virtual inline void drawMe();
		virtual inline bool isExpired();

	protected:
		int maxParticles;
		Vector initialPosition;
		int particlesPerTick;
		float particleLifeTime;
		GltColor* colorTransitions;
		float* colorTimeTransitions;
		int numColorTransitions;
		int textureNumber;
		Vector* sizeTransitions;
		float* sizeTimeTransitions;
		int numSizeTransitions;
		Vector initialVelocity;
		float totalLifeTime;
		float lifeTime;
		bool expired;

		int activeParticles;
		int curParticle;
		double initialTime;
		double lastTime;
		double tickTime;
		float curRelativeTime;

		Particle* particles;

		virtual inline void updateParticles();

		virtual inline void cullParticle(Particle* p);
		virtual inline void updateParticleTime(Particle* p);
		virtual inline void updateParticlePosition(Particle* p);
		virtual inline void updateParticleVelocity(Particle* p);
		virtual inline void updateParticleSize(Particle* p);
		virtual inline void updateParticleColor(Particle* p);
		virtual inline void updateParticleParameters(Particle* p);

		virtual inline int  getNumParticlesToEmit();
		virtual inline void emitParticles();
		virtual inline void initializeParticle(Particle* p);
		virtual inline void initializeParticleTime(Particle* p);
		virtual inline void initializeParticlePosition(Particle* p);
		virtual inline void initializeParticleVelocity(Particle* p);
		virtual inline void initializeParticleSize(Particle* p);
		virtual inline void initializeParticleColor(Particle* p);
		virtual inline void initializeParticleParameters(Particle* p);

		virtual inline bool updateReady();
		virtual inline void updateTime();

		virtual inline void setBlendMode();
		virtual inline void buildParticlePlane(Vector particlePos, Vector camera, Vector* n, Vector* t, Vector* b);
		virtual inline void drawLoop(Vector* n, Vector* t, Vector* b);

		void setColorTransition(GltColor* colorTransitions, float* colorTimeTransitions, int numColorTransitions);
		void setSizeTransition(Vector* sizeTransitions, float* sizeTimeTransitions, int numSizeTransitions);
		float getLifeTimeFraction();
};

#endif
