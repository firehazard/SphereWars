#ifndef POWERUPPARTICLEEFFECTS_H_
#define POWERUPPARTICLEEFFECTS_H_
#pragma once
#include "ParticleEffects.h"
#include <glt/color.h>
#include <math/vector3.h>
#include "Player.h"

class PowerupParticleEffects : public ParticleEffects
{
	public:
		PowerupParticleEffects(int particles, float rotations, float height, float samples, Player* player, float lifeTime, int texture);
		virtual ~PowerupParticleEffects(void);

	protected:
		Player* player;
		bool emitted;
		float rotations;
		float height;
		int currentInitIndex;
		float radiusBias;
		float samples;

		virtual inline int getNumParticlesToEmit();
		virtual inline void emitParticles();
		virtual inline void updateParticles();
		
		virtual inline void initializeParticleColor(Particle* p);
		virtual inline void initializeParticleParameters(Particle* p);

		virtual inline void updateParticleColor(Particle *p);
		virtual inline void updateParticleVelocity(Particle *p);
		virtual inline void updateParticleSize(Particle *p);
		virtual inline void updateParticleTime(Particle *p);

		virtual inline void drawLoop(Vector* n, Vector* t, Vector* b);
		virtual inline void getPosition(float angleOffset, float timeFraction, Vector* posOut);
		

};
#endif
