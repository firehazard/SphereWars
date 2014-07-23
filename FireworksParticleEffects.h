#ifndef FIREWORKSPARTICLEEFFECTS_H_
#define FIREWORKSPARTICLEEFFECTS_H_
#pragma once
#include "ParticleEffects.h"
#include <glt/color.h>
#include <math/vector3.h>

class FireworksParticleEffects : public ParticleEffects
{
	public:
		FireworksParticleEffects(int particles, Vector initialPosition, float lifeTime, int texture);
		virtual ~FireworksParticleEffects(void);

	protected:
		bool emitted;
		bool exploded;
		bool shouldExplode;
		GltColor emissionColor;
		Vector initialVelocity;
		float explodeDiameter;

		virtual inline int getNumParticlesToEmit();
		virtual inline void emitParticles();
		virtual inline void updateParticles();
		virtual inline void initializeParticleVelocity(Particle* p);
		virtual inline void initializeParticleColor(Particle* p);
		virtual inline void initializeParticleSize(Particle* p);

		virtual inline void updateParticleColor(Particle *p);
		virtual inline void updateParticleVelocity(Particle *p);
		virtual inline void updateParticleSize(Particle *p);
	
};
#endif
