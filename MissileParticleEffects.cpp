#include "MissileParticleEffects.h"
#include <math/vector3.h>
#include "SDLEngine.h"
#include <math.h>
#include "Utils.h"

MissileParticleEffects::MissileParticleEffects(Missile* missile)
{
	myMissile = missile;
	expired = false;
	isInitialized = false;
}

MissileParticleEffects::~MissileParticleEffects(void)
{
	myMissile = NULL;
	clear();
}

GLfloat** MissileParticleEffects::getPointers(int index) {
	GLfloat** pointers = (GLfloat**)malloc(sizeof(GLfloat*) * 4);
	pointers[0] = vertexPointer + index * 3;
	pointers[1] = colorPointer + index * 4;
	pointers[2] = velocityPointer + index * 3;
	pointers[3] = startTimePointer + index;

	return pointers;
}


void MissileParticleEffects::init() {
	maxParticles = 50;
	activeParticles = 0;
	curParticle = 0;
	initialTime = Utils::getTime();
	lastTime = initialTime;
	minTick = 0.01;
	curRelativeTime = 0.0;
	particleLifeTime = 0.10f;
	startColor = new GltColor(1.0, 1.0, 0.0, 1.0);
	endColor = new GltColor(1.0, 0.0, 0.0, 0.25);

	// allocate memory for the particles
	vertexPointer = new GLfloat[3 * maxParticles];
	memset(vertexPointer, 0, sizeof(GLfloat) * 3 * maxParticles);

	colorPointer = new GLfloat[4 * maxParticles];
	memset(colorPointer, 0, sizeof(GLfloat) * 4 * maxParticles);
	
	velocityPointer = new GLfloat[3 * maxParticles];
	memset(velocityPointer, 0, sizeof(GLfloat) * 3 * maxParticles);
	
	startTimePointer = new GLfloat[maxParticles];
	memset(startTimePointer, 0, sizeof(GLfloat) * maxParticles);
	isInitialized = true;
}

void MissileParticleEffects::clear()
{
	if (isInitialized) {
		delete startColor;
		delete endColor;
		delete[] vertexPointer;
		delete[] colorPointer;
		delete[] velocityPointer;
		delete[] startTimePointer;
		isInitialized = false;
	}	
}

void MissileParticleEffects::reset()
{
	clear();
	init();
}

void MissileParticleEffects::updateState() {
	if ((Utils::getTime() - lastTime) < minTick) {
		return;
	}
	// increment the time
	lastTime = Utils::getTime();
	curRelativeTime = lastTime - initialTime;

	updateColors();

	// activate another particle
	// choose a random number of particles to add
	double newParticles = (double)rand() / (double)RAND_MAX;
	newParticles *= 50;
	int numNewParticles = (int)floor(newParticles);
	for (int i = 0; i < numNewParticles; ++i) {
		if (!(curParticle < maxParticles)) {
			curParticle = 0;
		}
		float* newVertex = (vertexPointer + 3 * curParticle);
		float* newColor = (colorPointer + 4 * curParticle);
		float* newVelocity = (velocityPointer + 3 * curParticle);
		float* newTime = (startTimePointer + curParticle);

		*newVertex = myMissile->getPosition().getX();
		++newVertex;
		*newVertex = myMissile->getPosition().getY();
		++newVertex;
		*newVertex = myMissile->getPosition().getZ();


		// determine a velocity
		Vector missileInverseVelocity = myMissile->getVelocity().toVector();
		missileInverseVelocity = -missileInverseVelocity;

		double xVel = Utils::random();
		xVel *= 6 * Utils::randomSign();
		double yVel = Utils::random();
		yVel *= 6 * Utils::randomSign();
		double zVel = Utils::random();
		zVel *= 6 * Utils::randomSign();
		Vector particleVelocity(xVel, yVel, zVel);

		// now bias this velocity in the direction away from the missile
		particleVelocity += .5 * missileInverseVelocity;

		*newVelocity = particleVelocity.x();
		++newVelocity;
		*newVelocity = particleVelocity.y();
		++newVelocity;
		*newVelocity = particleVelocity.z();

		//add some randomness to the time
		double timeBias = Utils::random() / 10;
		*newTime = curRelativeTime - timeBias;
		*newColor = startColor->red();
		++newColor;
		*newColor = startColor->green();
		++newColor;
		*newColor = startColor->blue();
		++newColor;
		*newColor = startColor->alpha();

		++curParticle;
		if (activeParticles < maxParticles) {
			++activeParticles;
		}
	}
}

void MissileParticleEffects::updateColors() {
	GLfloat** pointers = getPointers(0);
	GLfloat* colors = pointers[1];
	GLfloat* time = pointers[3];
	GltColor colorDiff = *endColor - *startColor;

	for (int i = 0; i < activeParticles; ++i) {
		GLfloat lifeFrac = (curRelativeTime - *time) / particleLifeTime;
		GltColor diffColor = colorDiff * lifeFrac;
		GltColor newColor = *startColor + diffColor;
		*colors = newColor.red();
		++colors;
		*colors = newColor.green();
		++colors;
		*colors = newColor.blue();
		++colors;
		*colors = newColor.alpha();
		++colors;
		++time;
	}
}

void MissileParticleEffects::drawMe() {
	// set the time
	glUniform1fARB(S_UNI_POS[S_UNI_time], curRelativeTime);
	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, SDLEngine::textures[6]);
	glUniform1iARB(S_UNI_POS[S_UNI_tex0], 0);
	glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 1.0);
	glUniform1fARB(S_UNI_POS[S_UNI_shouldModulate], 1.0);
	glUniform1fARB(S_UNI_POS[S_UNI_lighting], 0.0);
	glUniform1fARB(S_UNI_POS[S_UNI_shouldParticle], 1.0);

	// setup pointers
	GLfloat** pointers = getPointers(0);
	GLfloat* vertices = pointers[0];
	GLfloat* colors = pointers[1];
	GLfloat* velocities = pointers[2];
	GLfloat* times = pointers[3];

	// get the vectors such that each particle is on a plane perpendicular to the eye vector
	// we'll just use the vector to the missile so we dont have to compute it for each particle
	Point camera = SDLEngine::getInstance()->getCameraLocation();
	Point missile(myMissile->getPosition().getX(), myMissile->getPosition().getY(), myMissile->getPosition().getZ());
	Vector n = Vector(camera.x() - missile.x(), camera.y() - missile.y(), camera.z() - missile.z());	
	n.normalize();

	// solve plane equation Ax + By + Cz + D = 0 =>  Ax + By + Cz = -D
	double d = -(n.x() * missile.x() + n.y() * missile.y() + n.z() * missile.z());
	// we know the plane equation now but we need another point on the plane to get a second vector
	// pick a random x and y
	double x = Utils::random();
	double y = Utils::random();
	// solve for z, -(Ax + By + D)/C = z
	double z = (-(n.x()*x + n.y()*y + d)) / n.z();

	// get a second vector in the plane with our new point, we'll call it the tangent
	Vector t(x - missile.x(), y - missile.y(), z - missile.z());
	t.normalize();
	// cross the two to get a third vector in the plane, call it binormal
	Vector b = xProduct(n, t);

	// use our new vectors to create our triangles so they are always perp to viewer
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < activeParticles; ++i) {
		glColor4f(*colors, *(colors+1), *(colors+2), *(colors+3));
		glVertexAttrib3fARB(S_ATT_vertexVelocity, *velocities, *(velocities+1), *(velocities+2));
		glVertexAttrib1fARB(S_ATT_vertexStartTime, *times);

		// first triangle
		glTexCoord2f(1, 1);
		glVertex3f((*vertices) + t.x() + b.x(), (*(vertices + 1)) + t.y() + b.y(), (*(vertices + 2)) + t.z() + b.z()); 
		glTexCoord2f(0, 1);
		glVertex3f((*vertices) - t.x() + b.x(), (*(vertices + 1)) - t.y() + b.y(), (*(vertices + 2)) - t.z() + b.z()); 
		glTexCoord2f(1, 0);
		glVertex3f((*vertices) + t.x() - b.x(), (*(vertices + 1)) + t.y() - b.y(), (*(vertices + 2)) + t.z() - b.z()); 
		
		// second triangle
		glTexCoord2f(1, 0);
		glVertex3f((*vertices) + t.x() - b.x(), (*(vertices + 1)) + t.y() - b.y(), (*(vertices + 2)) + t.z() - b.z()); 
		glTexCoord2f(0, 1);
		glVertex3f((*vertices) - t.x() + b.x(), (*(vertices + 1)) - t.y() + b.y(), (*(vertices + 2)) - t.z() + b.z()); 
		glTexCoord2f(0, 0);
		glVertex3f((*vertices) - t.x() - b.x(), (*(vertices + 1)) - t.y() - b.y(), (*(vertices + 2)) - t.z() - b.z()); 		

		vertices += 3;
		colors += 4;
		velocities += 3;
		++times;
	}
	glEnd();
	



	glUniform1fARB(S_UNI_POS[S_UNI_shouldTexture], 0.0);
	glUniform1fARB(S_UNI_POS[S_UNI_shouldModulate], 0.0);
	glUniform1fARB(S_UNI_POS[S_UNI_lighting], 0.0);
	glUniform1fARB(S_UNI_POS[S_UNI_shouldParticle], 0.0);
	free(pointers);
}

void MissileParticleEffects::setExpired(bool expired) {
	this->expired = expired;
}

bool MissileParticleEffects::getExpired() {
	return this->expired;
}
