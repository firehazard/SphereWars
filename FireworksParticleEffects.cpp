#include "FireworksParticleEffects.h"
#include "Utils.h"
#include <math/vector3.h>
#include "Constants.h"
#include "Sound.h"

FireworksParticleEffects::FireworksParticleEffects(int particles, Vector initialPosition, float lifeTime, int texture) : 
	ParticleEffects(particles, 0, initialPosition, lifeTime, texture, Vector(0,0,0), lifeTime) {
	
	emitted = false;
	exploded = false;
	shouldExplode = false;

	sizeTransitions = new Vector[1];
	*sizeTransitions = Vector(0.25, 0.25, 0.25);

	sizeTimeTransitions = (float*)malloc(sizeof(float) * 1);

	numSizeTransitions = 1;
	initialVelocity = Vector(0, 15 + Utils::random() * 50, 0);
	
	explodeDiameter = 20 + Utils::random() * 40;

}

FireworksParticleEffects::~FireworksParticleEffects(void)
{
}

int FireworksParticleEffects::getNumParticlesToEmit() {
	if (!emitted) {
		emitted = true;
		return maxParticles;
	} else {
		return 0;
	}
}

void FireworksParticleEffects::updateParticles() {
	if (shouldExplode) {
		shouldExplode = false;
		exploded = true;
		Sound::getInstance()->playSound(FIREWORKS_EXPLODE);
	}

	if (!exploded) {
		if (getLifeTimeFraction() < 0.6) {
			if (Utils::random() < 0.5) {
				shouldExplode = true;
			}
		}
	}

	ParticleEffects::updateParticles();
}

void FireworksParticleEffects::emitParticles() {
	// choose our color to emit then do it
	int initialColor = (rand() % 3);
	int secondColor = initialColor;
	while (initialColor == secondColor) {
		secondColor = (rand() % 3);
	}

	float red = 0;
	float green = 0;
	float blue = 0;

	switch (initialColor) {
		case 0:
			red = 1.0;
			break;
		case 1:
			blue = 1.0;
			break;
		case 2:
			green = 1.0;
			break;
	}


	switch (secondColor) {
		case 0:
			red = Utils::random();
			break;
		case 1:
			blue = Utils::random();
			break;
		case 2:
			green = Utils::random();
			break;
	}

	emissionColor = GltColor(red, green, blue, 1.0f);

	ParticleEffects::emitParticles();
}


void FireworksParticleEffects::initializeParticleVelocity(Particle* p) {
	// pick a random direction
	p->velocity = initialVelocity;
}

void FireworksParticleEffects::initializeParticleColor(Particle* p) {
	p->color = GltColor(1.0, 1.0, 1.0, 1.0);
}

void FireworksParticleEffects::updateParticleColor(Particle *p) {
	if (shouldExplode) {
		p->color = emissionColor;
	} else {
		if (p->time < (particleLifeTime / 2)) {
			// start fading out the alpha
			p->color = GltColor((float)p->color.red(), (float)p->color.green(), (float)p->color.blue(), (float)(p->time / (particleLifeTime / 2)));
		}
	}
}

void FireworksParticleEffects::updateParticleVelocity(Particle *p) {
	if (!exploded) {
		if (shouldExplode) {
			float u = 0.0;
			while (u < 0.01) {
				u = Utils::random() * 2*PI;
			}
			float v = 0.0;
			while (v < 0.01) {
				v = Utils::random() * 2*PI;
			} 

			p->velocity = Vector(sin(u)*sin(v) * explodeDiameter, cos(u)*sin(v) * explodeDiameter, cos(v)* explodeDiameter);
		} else {
			// just leave velocity alone
		}
	} else {
		float lifeTimeFraction = getLifeTimeFraction();
		if ((.45 < lifeTimeFraction) && (lifeTimeFraction < .5)) {
			p->velocity = Vector(0, 0, 0);
		} else if (lifeTimeFraction < .45) {
			p->velocity = Vector(0, p->velocity.y() - (Utils::random() * 2), 0);
		}


	}
}

void FireworksParticleEffects::initializeParticleSize(Particle *p) {
	p->size = Vector(0.05, 0.05, 0.05);
}

void FireworksParticleEffects::updateParticleSize(Particle *p) {
	if (exploded) {
		p->size = *sizeTransitions;
	}
}
