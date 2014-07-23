#include "Stage.h"
#include "SDLEngine.h"
#include "globals.h"
#include "Constants.h"
#include "Parallelogram.h"
#include "Triangle.h"
#include <assert.h>
#include <iostream>
#include <math.h>
using namespace std;

/** 
 * Notes on getting a random point:
 * "PickRandomPointAtHeight(int radius)"
 *  1. Pick a random parallelogram. 
 *   (Weighted by area, which = sqrt(|| vecA x vecB ||), I think)
 *  2. Pick a random point on that parallelogram. [0-1] * vecA + [0-1] * vecB
 *  3. Go one radius above that point, along direction of the normal.
 *    return the result
 */


#define DEPTH    50.0f

#define GROUND_WIDTH    20.0f
#define RAMP_WIDTH  14.142135
#define RAMP_HEIGHT 14.142135

#define STAGE_BOUNCINESS 1.0f // 1 is no-bouncing


#define PLATEAU_WIDTH  10.0f

#define BUMP_SEGMENT_WIDTH 0.3


#define ROTATION_Z  0.0f

// Shapes are allowed to sink this distance into the stage
// (necessary for preventing jitter and lack of friction)
#define MIN_SINKING  0.01f
#define MAX_SINKING  0.03f

const Vector3d Stage::gravity = Vector3d(0,GRAVITY,0);

Stage::Stage(void) 
{
	rot_angle = 0.0;
	stage_mind = new StageMind();
	rot_origin = Vector3d(0,0,0);	
	rot_axis = Vector3d(0,0,0);	
	force_draw = true;
	//initTriangleSurfaces();
	//initSurfaces();
	//initSimpleSurface();
	//rotateBy(ROTATION_Z,Vector3d::zAxis);
}

Stage::~Stage(void) {
	freeSurfaces();
}

void Stage::resetForceDraw()
{
	force_draw = false;
}

void Stage::freeSurfaces()
{
	for (int i = 0; i < (int)m_surfaces.size(); i++) {
		delete m_surfaces[i];
	}

	for (int i = 0; i < (int)m_surfacesOrig.size(); i++) {
		delete m_surfacesOrig[i];
	}
	m_surfaces.clear();
	m_surfacesOrig.clear();
}

void Stage::setSurfaces(const vector<FlatSurface*> surfaces)
{
	force_draw = true;
	freeSurfaces(); // Clear out any surfaces I'm currently using
	
	// Copy the passed-in surfaces
	for (int i = 0; i < (int)surfaces.size(); i++) {
		if (surfaces[i]->getType() == FlatSurface::TRIANGLE) {
			m_surfaces.push_back(new Triangle(*(Triangle*)surfaces[i]));
			m_surfacesOrig.push_back(new Triangle(*(Triangle*)surfaces[i]));
		} else if (surfaces[i]->getType() == FlatSurface::PARALLELOGRAM) {
			m_surfaces.push_back(new Parallelogram(*(Parallelogram*)surfaces[i]));
			m_surfacesOrig.push_back(new Parallelogram(*(Parallelogram*)surfaces[i]));
		} else {
			// Shouldn't get here
			assert(0);
		}
	}
}

void Stage::toggleGrass()
{
	for (int i = 0; i < (int)m_surfaces.size(); i++) {
		m_surfaces[i]->setTexture(SDLEngine::textures[14]);
		m_surfaces[i]->setBumpMapping(false);
	}
}

bool Stage::forceDraw()
{
	return force_draw;
}

void Stage::initSimpleSurface()
{
	double xpos = -(DEPTH/2);
    double ypos = 0;
    m_surfaces.push_back(new Parallelogram(Vector3d(xpos, ypos, DEPTH/2),
                                       Vector3d(DEPTH, 0, 0), 
                                       Vector3d(0, 0, -DEPTH)));
}

void Stage::initSurfaces()
{
	double xpos = -(GROUND_WIDTH + RAMP_WIDTH + PLATEAU_WIDTH/2);
	double ypos = 0;
	m_surfaces.push_back(new Parallelogram(Vector3d(xpos, ypos, DEPTH/2),
                                       Vector3d(GROUND_WIDTH, 0, 0), 
                                       Vector3d(0, 0, -DEPTH)));
	m_surfaces[0]->setTopTextureRepetition(2, 5);
	xpos += GROUND_WIDTH;
	                                       
	m_surfaces.push_back(new Parallelogram(Vector3d(xpos, ypos, DEPTH/2),
                                       Vector3d(RAMP_WIDTH, RAMP_HEIGHT, 0), 
                                       Vector3d(0, 0, -DEPTH)));
	m_surfaces[1]->setTopTextureRepetition(2, 5);
	xpos += RAMP_WIDTH;
	ypos += RAMP_HEIGHT;
	
	m_surfaces.push_back(new Parallelogram(Vector3d(xpos, ypos, DEPTH/2),
                                       Vector3d(PLATEAU_WIDTH, 0, 0), 
                                       Vector3d(0, 0, -DEPTH)));
	m_surfaces[2]->setTopTextureRepetition(2, 5);
	xpos += PLATEAU_WIDTH;	
                                      
	m_surfaces.push_back(new Parallelogram(Vector3d(xpos, ypos, DEPTH/2),
                                       Vector3d(RAMP_WIDTH, -RAMP_HEIGHT, 0), 
                                       Vector3d(0, 0, -DEPTH)));
	m_surfaces[3]->setTopTextureRepetition(2, 5);
	xpos += RAMP_WIDTH;
	ypos -= RAMP_HEIGHT;
	
	m_surfaces.push_back(new Parallelogram(Vector3d(xpos, ypos, DEPTH/2),
                                       Vector3d(GROUND_WIDTH, 0, 0), 
                                       Vector3d(0, 0, -DEPTH)));
	m_surfaces[4]->setTopTextureRepetition(2, 5);
	xpos += GROUND_WIDTH;

}

void Stage::initCurvedSurfaces()
{
	double xpos = -(GROUND_WIDTH + RAMP_WIDTH + PLATEAU_WIDTH/2);
	double ypos = 0;

	m_surfaces.push_back(new Parallelogram(Vector3d(xpos, ypos, DEPTH/2),
                                       Vector3d(GROUND_WIDTH, 0, 0), 
                                       Vector3d(0, 0, -DEPTH)));
	xpos += GROUND_WIDTH;
	                                       
	for (float angle = -PI/3; angle <= PI/3; angle += PI/1024) {
		float dx = cosf(angle) * BUMP_SEGMENT_WIDTH;
		float dy = sinf(angle) * BUMP_SEGMENT_WIDTH;
		m_surfaces.push_back(new Parallelogram(Vector3d(xpos, ypos, DEPTH/2),
    	                                   Vector3d(dx, dy, 0), 
        	                               Vector3d(0, 0, -DEPTH)));
		xpos += dx;
		ypos += dy;	
	}

	m_surfaces.push_back(new Parallelogram(Vector3d(xpos, ypos, DEPTH/2),
                                       Vector3d(GROUND_WIDTH, 0, 0), 
                                       Vector3d(0, 0, -DEPTH)));
	xpos += GROUND_WIDTH;

}

void Stage::initTriangleSurfaces()
{
    double ypos = 0;
    double height = 15;

	m_surfaces.push_back(new Triangle(Vector3d(-DEPTH/2, ypos, DEPTH/2),
                                           Vector3d(DEPTH, 0, 0), 
                                           Vector3d(DEPTH/2, height, -DEPTH/2)));

	m_surfaces.push_back(new Triangle(Vector3d(DEPTH/2, ypos, DEPTH/2),
                                           Vector3d(0, 0, -DEPTH), 
                                           Vector3d(-DEPTH/2, height, -DEPTH/2)));
	m_surfaces.push_back(new Triangle(Vector3d(-DEPTH/2, ypos, -DEPTH/2),
                                           Vector3d(0, 0, DEPTH), 
                                           Vector3d(DEPTH/2, height, DEPTH/2)));
	
	m_surfaces.push_back(new Triangle(Vector3d(DEPTH/2, ypos, -DEPTH/2),
                                           Vector3d(-DEPTH, 0, 0), 
                                           Vector3d(-DEPTH/2, height, DEPTH/2)));

}



void Stage::rotateBy(float angle, const Vector3d& axis)
{
	// Restore original surfaces
	force_draw = true;
	theWorld->rotatePowerupsBy(angle,axis);
	for (unsigned i = 0; i < m_surfaces.size(); i++) {
		m_surfaces[i]->reInitialize(*m_surfacesOrig[i]);
	}
	
	// Update member variables from args
	rot_axis = axis;
	rot_angle = angle;

	// Apply rotation	
	for (unsigned i = 0; i < m_surfaces.size(); i++) {
		m_surfaces[i]->rotate(-rot_angle, 
		                     rot_axis,
	                         rot_origin);
	}
}


void Stage::updateState(){
	stage_mind->updateState();
	//Friction and Gravity should be second to Last
	//Normal FORCE NEEDS TO BE LAST FORCED APPLIED (Ask T.D.)	
	for (unsigned i=0; i < theWorld->getNumPlayers(); i++) {
		Shape *shape = theWorld->getPlayer(i)->getShape();
		shape->applyForce(gravity.getScaledCopy(tick_delta));
		
		// Check if shape is intersecting the face of each surface
		// (and if so, correct for that) 
		shape->setContactingStage(false);
		shape->setLastStageNormal(Vector3d::zero);
		for (unsigned j = 0; j < m_surfaces.size(); j++) {
			// Check for shape touching stage
			if (isTouchingShape(shape, m_surfaces[j])) {
				handleContactingShape(shape, m_surfaces[j]->getNormal());
			} 
		}
		
		if (!shape->isContactingStage()) {
			// Check for shape intersecting stage polygon edges
			for (unsigned j = 0; j < m_surfaces.size(); j++) {
				for (unsigned k = 0; k < m_surfaces[j]->getNumEdges(); k++) {
					const LineSegment &edge = m_surfaces[j]->getEdge(k);
					if (isEdgeTouchingShape(shape, edge)) {
						handleContactingShape(shape, 
									edge.getNormalInDirectionOf(shape->getPosition()));
					}
				}
			}
		}

		// Now we know it's not overlapping stage with center above a surface / edge.
		// Check one more thing: vertices. (corner cases)		
		if (!shape->isContactingStage()) {
			for (unsigned j = 0; j < m_surfaces.size(); j++) {
				for (unsigned k = 0; k < m_surfaces[j]->getNumVertices(); k++) {
					const Vector3d &vertex = m_surfaces[j]->getVertex(k);
					if (isVertexTouchingShape(shape, vertex)) {
						handleContactingShape(shape, shape->getPosition()-vertex);
					}
				}
			}
		}
	}
	
}

void Stage::handleContactingShape(Shape *shape, const Vector3d &normal) const
{
	shape->setContactingStage(true);
	shape->setLastStageNormal(normal);
	shape->applyFriction();
	Vector3d vel =	shape->getVelocity(); 
	float force_mag = normal.getUnitVector().dotProduct(vel.getScaledCopy(-1));
	force_mag*=STAGE_BOUNCINESS;
	shape->applyForce(normal.getCopyWithMagnitude(force_mag));	
} 

void Stage::drawMe() {
	for (unsigned j = 0; j < m_surfaces.size(); j++) {
		m_surfaces[j]->drawMe();
	}
}


bool Stage::isTouchingShape(Shape* shape, const FlatSurface *surface) const
{
	double distance = fabs(surface->getDistanceBelow(shape->getPosition()));
	//printf("Distance: %f\n", distance); // DEBUG
	
	// TODO: This should be moved within Shape, 
	// or changed so it's not sphere-specific
	Sphere* sphere = dynamic_cast<Sphere*>(shape);
	assert(sphere); // make sure cast succeeded -- only supporting spheres for now
	
	if (distance < sphere->getRadius()) {
		if (surface->isInBounds(shape->getPosition())) {
			// Has shape sunk too deeply into stage?
			if (distance < sphere->getRadius() - MAX_SINKING) {
				// Adjust position of sphere so that its distance is 0
				Vector3d shift = 
			  		surface->getNormal().getCopyWithMagnitude(sphere->getRadius() - 
					                                           distance - MIN_SINKING);
				sphere->setPosition(sphere->getPosition() + shift);
			}
			return true;
		}		
	}	
	return false;
}

bool Stage::isEdgeTouchingShape(Shape *shape, const LineSegment &edge) const
{	
	float distance = edge.getDistanceToShape(shape);
	Sphere* sphere = dynamic_cast<Sphere*>(shape);
	assert(sphere); // make sure cast succeeded -- only supporting spheres for now

	float overlap = distance - sphere->getRadius();
	if (overlap < 0) {		
		// So, shape is within a radius of edge's infinite line =>
		// It could be touching an edge
		if (edge.isPointInBounds(shape->getPosition())) {
			backShapeAwayFromEdge(shape, edge, overlap);
			return true;
		}	 	
	}
	return false;
}

bool Stage::isVertexTouchingShape(Shape *shape, const Vector3d &vertex) const
{	
	float distance = (vertex - shape->getPosition()).getMagnitude();
	Sphere* sphere = dynamic_cast<Sphere*>(shape);
	assert(sphere); // make sure cast succeeded -- only supporting spheres for now

	float overlap = distance - sphere->getRadius();
	if (overlap < 0) {		
		backShapeAwayFromVertex(shape, vertex, -overlap);
		return true;
	}
	return false;
}


void Stage::backShapeAwayFromEdge(Shape *shape, const LineSegment &edge, float dist) const
{
	Vector3d normal = edge.getNormalInDirectionOf(shape->getPosition());
	Vector3d scaledNormal = normal.getCopyWithMagnitude(dist);
	shape->setPosition(shape->getPosition() +  scaledNormal);
}

void Stage::backShapeAwayFromVertex(Shape *shape, const Vector3d &vertex, float dist) const
{
	Vector3d normal = shape->getPosition() - vertex;
	Vector3d scaledNormal = normal.getCopyWithMagnitude(dist);
	shape->setPosition(shape->getPosition() +  scaledNormal);
}


double Stage::getFrictionAt(const Vector3d &position) const
{
	return tick_delta*2;
}

Vector3d Stage::getNormalFor(Shape* shape) const
{
	for (unsigned j = 0; j < m_surfaces.size(); j++) {
		if (isTouchingShape(shape, m_surfaces[j])) {
			return ( m_surfaces[j]->getNormal() );
		} 
	}
	return Vector3d(0,0,0);
}

bool Stage::isMissileTouchingStage(Missile* missile) 
{
	float radius = missile->getRadius() - 1; //Assume up/down radius smaller for now. TD
	for (unsigned j = 0; j < m_surfaces.size(); j++) {
		FlatSurface* surface = m_surfaces[j];
		double distance = fabs(surface->getDistanceBelow(missile->getPosition()));
		if (distance < radius) {
			if (surface->isInBounds(missile->getPosition())) {
				return true;
			}
		}
	}
	return false;
}

Vector3d Stage::getStageNormalForMissile(Missile* missile)
{
	float radius = missile->getRadius() - 1; //Assume up/down radius smaller for now. TD
	for (unsigned j = 0; j < m_surfaces.size(); j++) {
		FlatSurface* surface = m_surfaces[j];
		double distance = fabs(surface->getDistanceBelow(missile->getPosition()));
		if (distance < radius) {
			if (surface->isInBounds(missile->getPosition())) {
				return ( m_surfaces[j]->getNormal() );
			}
		}
	}
	return Vector3d(0,0,0);
}

Vector3d Stage::getRandomPosition()
{
	int x_loc = -20 + (rand()%40);
	int z_loc = -20 + (rand()%40);
	return Vector3d(x_loc,30,z_loc);
}
