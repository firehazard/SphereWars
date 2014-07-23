#include "PowerupParticleEffects.h"
#include "Utils.h"
#include <math/vector3.h>
#include "Constants.h"
#include "Shape.h"
#include "Sphere.h"

PowerupParticleEffects::PowerupParticleEffects(int particles, float rotations, float height, float samples, Player* player, float lifeTime, int texture) : 
	ParticleEffects(particles, 0, initialPosition, lifeTime, texture, Vector(0,0,0), lifeTime) {
	
	this->player = player;
	this->rotations = rotations;
	this->height = height;
	emitted = false;

	sizeTransitions = new Vector[1];
	*sizeTransitions = Vector(0.5, 0.5, 0.5);

	sizeTimeTransitions = (float*)malloc(sizeof(float) * 1);

	numSizeTransitions = 1;
	currentInitIndex = 0;

	radiusBias = 1.04;
	this->samples = samples;
}

PowerupParticleEffects::~PowerupParticleEffects(void)
{
}

int PowerupParticleEffects::getNumParticlesToEmit() {
	if (!emitted) {
		emitted = true;
		return maxParticles;
	} else {
		return 0;
	}
}

void PowerupParticleEffects::updateParticles() {
	ParticleEffects::updateParticles();
}

void PowerupParticleEffects::emitParticles() {
	ParticleEffects::emitParticles();
}


void PowerupParticleEffects::initializeParticleColor(Particle* p) {
	if (currentInitIndex % 2) {
		p->color = GltColor(1.0, 0.0, 0.0, 1.0);
	} else {
		p->color = GltColor(0.0, 0.0, 1.0, 1.0);
	}	
}

void PowerupParticleEffects::initializeParticleParameters(Particle* p) {
	p->f1 = ((2 * PI) / (double)maxParticles) * currentInitIndex;

	Shape* shape = player->getShape();
	Sphere* sphere = dynamic_cast<Sphere*>(shape);
	
	if (sphere) {
		double radius	= sphere->getRadius();
		float lifeFraction = 1 - getLifeTimeFraction();
		float angle = (2 * PI * rotations * lifeFraction);
		p->pos = Vector((radiusBias * radius) * cos(angle + p->f1) + player->getShape()->getPosition().getX(), 
			height * lifeFraction + player->getShape()->getPosition().getY() - radius + 0.25, 
			(radiusBias * radius) * sin(angle + p->f1) + player->getShape()->getPosition().getZ());
	}

	++currentInitIndex;
}


void PowerupParticleEffects::updateParticleColor(Particle *p) {
	if (p->time < (particleLifeTime / 2)) {
		// start fading out the alpha
		p->color = GltColor((float)p->color.red(), (float)p->color.green(), (float)p->color.blue(), (float)(p->time / (particleLifeTime / 2)));
	}
}

void PowerupParticleEffects::updateParticleVelocity(Particle *p) {
}

void PowerupParticleEffects::updateParticleSize(Particle *p) {
	// stay same size
}

void PowerupParticleEffects::updateParticleTime(Particle *p) {
	ParticleEffects::updateParticleTime(p);
	
	getPosition(p->f1, 1 - getLifeTimeFraction(), &(p->pos));
}

void PowerupParticleEffects::getPosition(float angleOffset, float timeFraction, Vector* posOut) {
	// update position with 
	Shape* shape = player->getShape();
	Sphere* sphere = dynamic_cast<Sphere*>(shape);
	
	if (sphere) {
		double radius	= sphere->getRadius();
		float angle = (2 * PI * rotations * timeFraction);
		float distance  = radiusBias * radius;

		if (timeFraction > 0.8) {
			distance = distance * (((1 - timeFraction) / 2) * 10);
		}

		*posOut = Vector(distance * cos(angle + angleOffset) + player->getShape()->getPosition().getX(), 
			height * timeFraction + player->getShape()->getPosition().getY() - radius + 0.25, 
			distance * sin(angle + angleOffset) + player->getShape()->getPosition().getZ());
	}
}


void PowerupParticleEffects::drawLoop(Vector* n, Vector* t, Vector* b) {
	// use our new vectors to create our triangles so they are always perp to viewer
	Particle* p = particles;
	Particle* pEnd = particles + maxParticles;
	float lifeTime = 1 - getLifeTimeFraction();
	Sphere* sphere = dynamic_cast<Sphere*>(player->getShape());
	double radius	= sphere->getRadius();

	glBegin(GL_TRIANGLES);
	while (p != pEnd) {
		if (p->alive) {
			// draw 5 particles per particle with slightly decreasing times
			for (int i = samples; i >= 0; --i) {
				float timeFrac = lifeTime - ((1 / radius) * 0.014 * i);
				if (timeFrac < 0.0) {
					continue;
				}
				//printf("%f\n", cos(((float)i / 7) * (PI/2)));
				//float alpha = p->color.alpha() * cos(((float)i / 16) * (PI/2));
				float alpha = p->color.alpha() * exp(-5 * ((float)i / 30));
				//cout << p->color.alpha() << " " << alpha << " i: " << i << endl;
				Vector pos;
				getPosition(p->f1, timeFrac, &pos);
				//printf("Drawing particle: X: %f Y: %f Z: %f\n", p->pos.x() + t.x() + b.x(), p->pos.y() + t.y() + b.y(), p->pos.z() + t.z() + b.z());
				//printf("Color: R: %f G: %f B: %f A: %f\n", p->color.red(), p->color.green(), p->color.blue(), p->color.alpha());
				//cout << "T Before: " << t << endl;
				//cout << p->size.x() << endl;
				Vector tPrime = p->size.x() * (*t);
				//cout << "T After: " << tPrime << endl;
				Vector bPrime = p->size.y() * (*b);
				if (i == 0) {
					// set lead particles slightly larger
					tPrime.scale(1.5);
					bPrime.scale(1.5);
				}

				glColor4f(p->color.x(), p->color.y(), p->color.z(), alpha);
				glTexCoord2f(1, 1);
				glVertex3f(pos.x() + tPrime.x() + bPrime.x(), pos.y() + tPrime.y() + bPrime.y(), pos.z() + tPrime.z() + bPrime.z()); 
				glTexCoord2f(0, 1);
				glVertex3f(pos.x() - tPrime.x() + bPrime.x(), pos.y() - tPrime.y() + bPrime.y(), pos.z() - tPrime.z() + bPrime.z()); 
				glTexCoord2f(1, 0);
				glVertex3f(pos.x() + tPrime.x() - bPrime.x(), pos.y() + tPrime.y() - bPrime.y(), pos.z() + tPrime.z() - bPrime.z()); 

				// draw second tri
				glTexCoord2f(1, 0);
				glVertex3f(pos.x() + tPrime.x() - bPrime.x(), pos.y() + tPrime.y() - bPrime.y(), pos.z() + tPrime.z() - bPrime.z()); 
				glTexCoord2f(0, 1);
				glVertex3f(pos.x() - tPrime.x() + bPrime.x(), pos.y() - tPrime.y() + bPrime.y(), pos.z() - tPrime.z() + bPrime.z()); 
				glTexCoord2f(0, 0);
				glVertex3f(pos.x() - tPrime.x() - bPrime.x(), pos.y() - tPrime.y() - bPrime.y(), pos.z() - tPrime.z() - bPrime.z()); 		

			}
		}
		++p;
	}
	glEnd();
}
