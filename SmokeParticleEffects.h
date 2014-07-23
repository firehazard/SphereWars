#ifndef SMOKEPARTICLEEFFECTS_H
#define SMOKEPARTICLEEFFECTS_H
#pragma once
#include "ParticleEffects.h"

class SmokeParticleEffects : public ParticleEffects
{
	public:
		SmokeParticleEffects(int maxParticles, Vector initialPosition, int textureNumber);
		virtual ~SmokeParticleEffects(void);

	protected:
		virtual inline void initializeParticlePosition(Particle* p);
		virtual inline void initializeParticleVelocity(Particle* p);
		virtual inline void initializeParticleTime(Particle* p);
		virtual inline void initializeParticleSize(Particle* p);
		virtual inline void updateParticleVelocity(Particle* p);
		virtual inline void updateParticleSize(Particle* p);

		virtual inline int getNumParticlesToEmit();

		virtual inline void setBlendMode();

		bool emitted;


};

#endif
