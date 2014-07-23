#include "SmokeParticleEffects.h"
#include "Utils.h"

SmokeParticleEffects::SmokeParticleEffects(int maxParticles, Vector initialPosition,
																					 int textureNumber) : ParticleEffects(maxParticles, 
																					 particlesPerTick, initialPosition, 0.75, textureNumber, Vector(0, 15, 0), 0.75)
{
	colorTransitions = new GltColor[3];
	*colorTransitions = GltColor(0.4, 0.4, 0.4, 0.5);
	(*(colorTransitions+1)) = GltColor(0.4, 0.4, 0.4, 0.5);
	(*(colorTransitions+2)) = GltColor(0.4, 0.4, 0.4, 0.15);
	
	colorTimeTransitions = (float*)malloc(sizeof(float) * 3);
	*colorTimeTransitions = 0.0;
	(*(colorTimeTransitions + 1)) = 0.75;
	(*(colorTimeTransitions + 2)) = 1.0;
	
	numColorTransitions = 3;

	sizeTransitions = new Vector[3];
	*sizeTransitions = Vector(1.0, 1.0, 1.0);
	(*(sizeTransitions + 1)) = Vector(1.0, 1.0, 1.0);
	(*(sizeTransitions + 2)) = Vector(1.0, 1.0, .0);

	sizeTimeTransitions = (float*)malloc(sizeof(float) * 3);
	*sizeTimeTransitions = 0.0;
	(*(sizeTimeTransitions+1)) = 0.5;
	(*(sizeTimeTransitions+2)) = 1.0;

	numSizeTransitions = 3;
	tickTime = 0.05;

	emitted = false;
}

SmokeParticleEffects::~SmokeParticleEffects(void)
{
}

void SmokeParticleEffects::initializeParticlePosition(Particle* p) {
	ParticleEffects::initializeParticlePosition(p);
	// add some randomness to the starting location
	p->pos += Vector(Utils::randomSign() * Utils::random() * 2, Utils::random(), Utils::randomSign() * Utils::random() * 2);
}

void SmokeParticleEffects::initializeParticleVelocity(Particle* p) {
	ParticleEffects::initializeParticleVelocity(p);
	// get a random velocity element and add it to ours
	p->velocity += Vector(Utils::randomSign() * Utils::random() * 5, Utils::randomSign() * Utils::random() * 2, Utils::randomSign() * Utils::random() * 5);
}

void SmokeParticleEffects::initializeParticleTime(Particle* p) {
	p->time = particleLifeTime - (0.25 * particleLifeTime * Utils::random());
}

void SmokeParticleEffects::initializeParticleSize(Particle* p) {
	double rand = Utils::random() * 4;

	if (rand < 2.0) {
		rand = 2.0;
	}
	p->size = Vector(rand, rand, rand);
}

void SmokeParticleEffects::updateParticleVelocity(Particle* p) {
	// slowly remove x and y velocity components from  the velocity
	//p->velocity = Vector(p->velocity.x() * 0.97, p->velocity.y(), p->velocity.z() * 0.97);
	p->velocity = Vector(p->velocity.x() + Utils::random() * Utils::randomSign(), p->velocity.y(), p->velocity.z() + Utils::random() * Utils::randomSign());
}

void SmokeParticleEffects::updateParticleSize(Particle *p) {
	// null
}

void SmokeParticleEffects::setBlendMode() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int SmokeParticleEffects::getNumParticlesToEmit() {
	if (!emitted) {
		emitted = true;
		return maxParticles;
	} else {
		return 0;
	}
}