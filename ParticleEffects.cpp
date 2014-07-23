#include "ParticleEffects.h"
#include "Utils.h"
#include "SDLEngine.h"

ParticleEffects::ParticleEffects(int maxParticles, int particlesPerTick,
																 Vector initialPosition, float particleLifeTime, 
																 int textureNumber, Vector initialVelocity, 
																 float lifeTime) {
	this->maxParticles = maxParticles;
	this->particlesPerTick = particlesPerTick;
	this->initialPosition = initialPosition;
	this->particleLifeTime = particleLifeTime;
	this->colorTransitions = NULL;
	this->colorTimeTransitions = NULL;
	this->numColorTransitions = 0;
	this->textureNumber = textureNumber;
	this->sizeTransitions = NULL;
	this->sizeTimeTransitions = NULL;
	this->numSizeTransitions = 0;
	this->initialVelocity = initialVelocity;
	this->lifeTime = lifeTime;
	this->totalLifeTime = lifeTime; // immutable
	expired = false;

	activeParticles = 0;
	curParticle = 0;
	tickTime = 0.05f;
	curRelativeTime = 0;
	particleLifeTime = 0;

	initialTime = Utils::getTime();
	lastTime = initialTime;

	// allocate memory for all our particles
	particles = (Particle*)malloc(sizeof(Particle) * maxParticles);
	memset(particles, 0, sizeof(Particle) * maxParticles);
}

ParticleEffects::~ParticleEffects(void)
{
	free(particles);
	delete[] colorTransitions;
	free(colorTimeTransitions);
	delete[] sizeTransitions;
	free(sizeTimeTransitions);
}

void ParticleEffects::cullParticle(Particle* p) {
	if (p->alive) {
		if (p->time <= 0.0) {
			p->alive = false;
		}
	}
}

void ParticleEffects::updateState() {
	if (!updateReady()) {
		return;
	}

	updateParticles();

	emitParticles();

	updateTime();
}

void ParticleEffects::updateParticles() {
	bool allParticlesDead = true;
	Particle* p = particles;
	for (int i = 0; i < maxParticles; ++i) {
		if (p->alive) {
			allParticlesDead = false;
			updateParticleTime(p);
			cullParticle(p);
			updateParticlePosition(p);
			updateParticleVelocity(p);
			updateParticleSize(p);
			updateParticleColor(p);
			updateParticleParameters(p);
		}
		++p;
	}

	if (allParticlesDead && (lifeTime <= 0.0f)) {
		expired = true;
	}
}

void ParticleEffects::updateParticlePosition(Particle* p) {
	p->pos += p->velocity * tickTime;
}

void ParticleEffects::updateParticleVelocity(Particle* p) {
	// default implementation assumes static velocity
}

void ParticleEffects::updateParticleSize(Particle* p) {
	// determine what fraction of life this particle is in, from (0,1)
	float lifeFrac = (particleLifeTime - p->time) / particleLifeTime;

	// find which size we should be between for this time
	Vector* size = sizeTransitions;
	float* time = sizeTimeTransitions;

	//cout << "LifeFrac: " << lifeFrac << endl;
	for (int i = 0; i < (numSizeTransitions - 1); ++i) {
		//cout << *time << " " << (*(time+1)) << endl;
		if (((*time) <= lifeFrac) && (lifeFrac < (*(time+1)))) {
			//cout << "FOUND SIZE UPDATE!" << endl;
			Vector sizeDiff = (*(size+1)) - (*size);
			sizeDiff *= ((lifeFrac - *time) / ((*(time+1)) - (*time)));
			p->size = (*size) + sizeDiff;			
		}

		++size;
		++time;
	}
}

void ParticleEffects::updateParticleColor(Particle* p) {
	// determine what fraction of life this particle is in, from (0,1)
	float lifeFrac = (particleLifeTime - p->time) / particleLifeTime;
	//cout << "Life Frac: " << lifeFrac << endl;
	// find which color we should be between for this time
	GltColor* color = colorTransitions;
	float* time = colorTimeTransitions;

	for (int i = 0; i < (numColorTransitions - 1); ++i) {
		if (((*time) <= lifeFrac) && (lifeFrac < (*(time+1)))) {
			//cout << "Found Color Range" << endl;
			//cout << "Before Color: " << p->color.red() << " " << p->color.green() << " " << p->color.blue() << " " << p->color.alpha() << endl;
			GltColor colorDiff = (*(color+1)) - (*color);
			//cout << "Diff: " << ((lifeFrac - *time) / ((*(time+1)) - (*time))) << endl;
			//cout << "Diff Color Before: " << colorDiff.red() << " " << colorDiff.green() << " " << colorDiff.blue() << " " << colorDiff.alpha() << endl;
			//cout << "Alpha Before: " << p->color.alpha();
			colorDiff = colorDiff * ((lifeFrac - *time) / ((*(time+1)) - (*time)));
			//cout << "Alpha After: " << p->color.alpha();
			//cout << "Diff Color: " << colorDiff.red() << " " << colorDiff.green() << " " << colorDiff.blue() << " " << colorDiff.alpha() << endl;
			p->color = (*color) + colorDiff;			
			//cout << "After Color: " << p->color.red() << " " << p->color.green() << " " << p->color.blue() << " " << p->color.alpha() << endl;
		}

		++color;
		++time;
	}
}

void ParticleEffects::updateParticleTime(Particle* p) {
	p->time -= tickTime;
}

void ParticleEffects::updateParticleParameters(Particle* p) {
	// nothing in the default version, not using the extra parameters
}

int ParticleEffects::getNumParticlesToEmit() {
	return particlesPerTick;
}

void ParticleEffects::emitParticles() {
	if (lifeTime <= 0.0f) {
		return;
	}
	Particle* p = particles;
	Particle* pEnd = (particles + maxParticles);
	int numToEmit = getNumParticlesToEmit();
	for (int i = 0; i < numToEmit; ++i) {
		// scan through and find the first dead particle
		bool initializedParticle = false;

		while (p != pEnd) {
			if (p->alive) {
				++p;
				continue;
			} else {
				initializeParticle(p);
				initializedParticle = true;
				break;
			}
		}

		if (!initializedParticle) {
			break;
		}
	}
}

void ParticleEffects::initializeParticle(Particle* p) {
	p->alive = true;
	initializeParticleTime(p);
	initializeParticlePosition(p);
	initializeParticleVelocity(p);
	initializeParticleSize(p);
	initializeParticleColor(p);
	initializeParticleParameters(p);
}

void ParticleEffects::initializeParticleTime(Particle* p) {
	p->time = particleLifeTime;
}

void ParticleEffects::initializeParticlePosition(Particle* p) {
	p->pos = initialPosition;
}

void ParticleEffects::initializeParticleVelocity(Particle* p) {
	p->velocity = initialVelocity;	
}

void ParticleEffects::initializeParticleSize(Particle* p) {
	p->size = *sizeTransitions;
}

void ParticleEffects::initializeParticleColor(Particle* p) {
	p->color = *colorTransitions;
}

void ParticleEffects::initializeParticleParameters(Particle* p) {
	// no initial extra particle parameters
}

void ParticleEffects::buildParticlePlane(Vector particlePos, Vector camera, Vector* n, Vector* t, Vector* b) {

	*n = Vector(camera.x() - particlePos.x(), camera.y() - particlePos.y(), camera.z() - particlePos.z());	
	n->normalize();

	// solve plane equation Ax + By + Cz + D = 0 =>  Ax + By + Cz = -D
	double d = -(n->x() * particlePos.x() + n->y() * particlePos.y() + n->z() * particlePos.z());
	// we know the plane equation now but we need another point on the plane to get a second vector
	// pick a random x and y
	double x = Utils::random();
	double y = Utils::random();
	// solve for z, -(Ax + By + D)/C = z
	double z = (-(n->x()*x + n->y()*y + d)) / n->z();

	// get a second vector in the plane with our new point, we'll call it the tangent
	*t = Vector(x - particlePos.x(), y - particlePos.y(), z - particlePos.z());
	t->normalize();
	// cross the two to get a third vector in the plane, call it binormal
	*b = xProduct(*n, *t);
}


void ParticleEffects::drawMe() {
	setBlendMode();
	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, SDLEngine::textures[textureNumber]);
	
	glUniform1iARB(S_UNI_POS[S_UNI_tex0], 0);
	glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 1.0);
	glUniform1fARB(S_UNI_POS[S_UNI_shouldModulate], 1.0);

	// setup pointers

	// get the vectors such that each particle is on a plane perpendicular to the eye vector
	// we'll just use the vector to the initial position so we dont have to compute it for each particle
	Point camera = SDLEngine::getInstance()->getCameraLocation();
	Vector n, t, b;
	buildParticlePlane(initialPosition, Vector(camera), &n, &t, &b);
	
	drawLoop(&n, &t, &b);

	glUniform1fARB(S_UNI_POS[S_UNI_shouldModulate], 0.0);
	glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 0.0);
	
}

void ParticleEffects::drawLoop(Vector* n, Vector* t, Vector* b) {
	// use our new vectors to create our triangles so they are always perp to viewer
	Particle* p = particles;
	Particle* pEnd = particles + maxParticles;

	glBegin(GL_TRIANGLES);
	while (p != pEnd) {
		if (p->alive) {
			//printf("Drawing particle: X: %f Y: %f Z: %f\n", p->pos.x() + t.x() + b.x(), p->pos.y() + t.y() + b.y(), p->pos.z() + t.z() + b.z());
			//printf("Color: R: %f G: %f B: %f A: %f\n", p->color.red(), p->color.green(), p->color.blue(), p->color.alpha());
			//cout << "T Before: " << t << endl;
			//cout << p->size.x() << endl;
			Vector tPrime = p->size.x() * (*t);
			//cout << "T After: " << tPrime << endl;
			Vector bPrime = p->size.y() * (*b);

			glColor4f(p->color.x(), p->color.y(), p->color.z(), p->color.alpha());
			glTexCoord2f(1, 1);
			glVertex3f(p->pos.x() + tPrime.x() + bPrime.x(), p->pos.y() + tPrime.y() + bPrime.y(), p->pos.z() + tPrime.z() + bPrime.z()); 
			glTexCoord2f(0, 1);
			glVertex3f(p->pos.x() - tPrime.x() + bPrime.x(), p->pos.y() - tPrime.y() + bPrime.y(), p->pos.z() - tPrime.z() + bPrime.z()); 
			glTexCoord2f(1, 0);
			glVertex3f(p->pos.x() + tPrime.x() - bPrime.x(), p->pos.y() + tPrime.y() - bPrime.y(), p->pos.z() + tPrime.z() - bPrime.z()); 

			// draw second tri
			glTexCoord2f(1, 0);
			glVertex3f(p->pos.x() + tPrime.x() - bPrime.x(), p->pos.y() + tPrime.y() - bPrime.y(), p->pos.z() + tPrime.z() - bPrime.z()); 
			glTexCoord2f(0, 1);
			glVertex3f(p->pos.x() - tPrime.x() + bPrime.x(), p->pos.y() - tPrime.y() + bPrime.y(), p->pos.z() - tPrime.z() + bPrime.z()); 
			glTexCoord2f(0, 0);
			glVertex3f(p->pos.x() - tPrime.x() - bPrime.x(), p->pos.y() - tPrime.y() - bPrime.y(), p->pos.z() - tPrime.z() - bPrime.z()); 		
		}
		++p;
	}
	glEnd();
}


bool ParticleEffects::updateReady() {
	if ((Utils::getTime() - lastTime) < tickTime) {
		return false;
	}
	return true;
}

void ParticleEffects::updateTime() {
	lastTime = Utils::getTime();
	curRelativeTime = lastTime - initialTime;
	lifeTime -= tickTime;
}

void ParticleEffects::setColorTransition(GltColor* colorTransitions, float* colorTimeTransitions, int numColorTransitions) {
	this->colorTransitions = colorTransitions;
	this->colorTimeTransitions = colorTimeTransitions;
	this->numColorTransitions = numColorTransitions;
}

void ParticleEffects::setSizeTransition(Vector *sizeTransitions, float *sizeTimeTransitions, int numSizeTransitions) {
	this->sizeTransitions = sizeTransitions;
	this->sizeTimeTransitions = sizeTimeTransitions;
	this->numSizeTransitions = numSizeTransitions;
}

bool ParticleEffects::isExpired() {
	return expired;
}

void ParticleEffects::setBlendMode() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

float ParticleEffects::getLifeTimeFraction() {
	return (lifeTime / totalLifeTime);
}
