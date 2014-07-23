#include "FlatSurface.h"
#include "Constants.h"
#include <assert.h>
#include <string>
using namespace std;

const string FlatSurface::s_flatSurfaceNames[] = {"triangle", "parallelogram"};

FlatSurface::FlatSurfaceType FlatSurface::getTypeForName(const string &surfaceName)
{
	// Linear lookup -- not optimal, but there are few types of powerups
	// and this only needs to be called when loading a level.
	unsigned i;
	for (i = 0; i < NUM_FLAT_SURFACE_TYPES; i++) {
		if (surfaceName == s_flatSurfaceNames[i]) {
			break;
		}
	}
	return (FlatSurfaceType)i;
}

string FlatSurface::getNameForType(FlatSurface::FlatSurfaceType type)
{
	// Linear lookup -- not optimal, but there are few types of powerups
	// and this only needs to be called when loading a level.
	return s_flatSurfaceNames[type];
}


FlatSurface::FlatSurface(const Vector3d &origin, const Vector3d &vecA, 
                         const Vector3d &vecB)
	: m_origin(origin), m_vecA(vecA), m_vecB(vecB)
{
	// Sanity check on flat surface type names -- make sure we have the right number.
	assert(sizeof(s_flatSurfaceNames) / sizeof(string) == NUM_FLAT_SURFACE_TYPES);
	
	updateNormal();
	updateCobMatrix();
	// updateEdges();-- Needs to be called in child
	topTextureRepetitionX = 1.0;
	topTextureRepetitionY = 1.0;
	m_texture = INVALID_TEXTURE;
}

FlatSurface::FlatSurface(const FlatSurface &other)
{
	this->m_origin = other.m_origin;
	this->m_vecA   = other.m_vecA;
	this->m_vecB   = other.m_vecB;
	this->topTextureRepetitionX = other.topTextureRepetitionX;
	this->topTextureRepetitionY = other.topTextureRepetitionY;
	updateNormal();
	updateCobMatrix();
	// updateEdges(); -- Needs to be called in child class
	
	// Copy Texture Coordinates
	for (int i = 0; i < (int)other.m_texCoords.size(); i++) {
		this->m_texCoords.push_back(other.m_texCoords[i]);
	}
	m_texture = other.m_texture;
}

//SHOULD NOT be called from a constructor, because it calls updateEdges,
// which is defined in child class.
const void FlatSurface::reInitialize(FlatSurface &other)
{
	this->m_origin = other.m_origin;
	this->m_vecA   = other.m_vecA;
	this->m_vecB   = other.m_vecB;
	this->topTextureRepetitionX = other.topTextureRepetitionX;
	this->topTextureRepetitionY = other.topTextureRepetitionY;
	updateNormal();
	updateCobMatrix();
	updateEdges();
}

const Vector3d &FlatSurface::getNormal() const
{
	return m_normal; 
}

const Vector3d &FlatSurface::getOrigin() const
{
	return m_origin;
}

const Vector3d &FlatSurface::getVecA() const
{
	return m_vecA;
}
const Vector3d &FlatSurface::getVecB() const
{
	return m_vecB;
}

LineSegment FlatSurface::getEdge(unsigned index) const
{
	assert(index < m_edges.size());
	return m_edges[index];
}

void FlatSurface::setTopTextureRepetition(float x, float y) {
	this->topTextureRepetitionX = x;
	this->topTextureRepetitionY = y;
}

double FlatSurface::getDistanceBelow(const Vector3d &position) const
{
	const Vector3d shiftedShapePos = position - m_origin;
	const Vector3d unitNormal = m_normal.getUnitVector();
	
	return unitNormal.getX() * shiftedShapePos.getX() +
		unitNormal.getY() * shiftedShapePos.getY() +
		unitNormal.getZ() * shiftedShapePos.getZ();
	// In general you have do divide distance by magnitude of the normal,
	// but since we're using a unit normal, we don't need to.
}

void FlatSurface::rotate(float rot_angle, 
                      const Vector3d &rot_axis, 
                      const Vector3d &rot_origin)
{
	float origMagA = m_vecA.getMagnitude();
	float origMagB = m_vecB.getMagnitude();

	// Compute rotations of the two vertices bordering the origin
	// (It's important that the getVertex() calls happen before we 
	// rotate the origin -- DWH)
	Vector3d vertA = getVertex(1);
	Vector3d vertB = getVertex(getNumVertices() - 1);
	
	vertA.rotateBy(rot_angle, rot_axis, rot_origin);	
	vertB.rotateBy(rot_angle, rot_axis, rot_origin);
	
	m_origin.rotateBy(rot_angle, rot_axis, rot_origin);
	
	// Subtract rotated origin from rotated vertices to get new
	// directional vectors
	m_vecA = (vertA - m_origin).getCopyWithMagnitude(origMagA);	
	m_vecB = (vertB - m_origin).getCopyWithMagnitude(origMagB);
	
	updateNormal();
	updateCobMatrix();
	updateEdges();
}

void FlatSurface::setTexture(GLuint textureIndex)
{
	m_texture = textureIndex;
}

void FlatSurface::setTextureCoord(int vertexIndex, const Vector3d &coord)
{
	assert(vertexIndex < (int)getNumVertices());
	if (m_texCoords.size() != getNumVertices()) {
		m_texCoords.resize(getNumVertices());
	}
	m_texCoords[vertexIndex] = coord;
}

void FlatSurface::setBumpMapping(bool isBumpMapping)
{
	m_isBumpMapping = isBumpMapping;
}

void FlatSurface::updateNormal()
{
	m_normal = m_vecA.crossProduct(m_vecB);	
}


void FlatSurface::updateCobMatrix()
{
	Vector3d basisVecs[3];
	float buffer[16];
	basisVecs[0] = m_vecA.getUnitVector();
	basisVecs[1] = m_vecB.getUnitVector();
	basisVecs[2] = m_normal.getUnitVector();
	
	memset(buffer, 0, sizeof(buffer));
	
	// Populate the columns
	for (int i = 0; i < 3; i++) {		
		int baseIndex = i * 4;
		buffer[baseIndex]     = basisVecs[i].getX();
		buffer[baseIndex + 1] = basisVecs[i].getY();
		buffer[baseIndex + 2] = basisVecs[i].getZ();
	}
	buffer[15] = 1; // so it's invertible
	
	Matrix inverseCobMatrix(buffer);
	m_cobMatrix = inverseCobMatrix.inverse(); 
}

void FlatSurface::updateEdges()
{
	// Defined in subclass	
}

/*
 * Returns the closest point on the plane of this surface
 * to the given point. Note that this closest point may actually
 * be out of bounds of the surface.
 * 
 * Equation: closest = pt - (normal * pt / ||normal||^2) normal
 */
Vector3d FlatSurface::getClosestPointTo(const Vector3d &pt) const
{
	// Shift coordinate system so origin is at (0,0,0)
	const Vector3d shiftedPt = pt - m_origin; 
	const float scalar = m_normal.dotProduct(shiftedPt) / m_normal.getSquareMagnitude();
	const Vector3d shiftedResult = shiftedPt - m_normal.getScaledCopy(scalar);
	return shiftedResult + m_origin;
}

/*
 * Convert from triOrigin to "point" to be in the coordinate system of the
 * triangle's basis vectors, and check whether its coefficients are non-negative and
 * sum to at most 1. If so, it's in the triangle.
 */ 
bool FlatSurface::isPointInTriangleBounds(const Vector3d &point, const Vector3d &triOrigin,
                                          const Vector3d &triVecA, const Vector3d &triVecB,
                                          const Matrix &cobMatrix, bool flipSign)
{
	const Vector vec = (point - triOrigin).toVector();
	Vector cobVec = cobMatrix * vec;
	if (flipSign) {
		cobVec = -cobVec;
	}
	if (cobVec[0] > 0 && cobVec[1] > 0) {
		float fractionOfA = cobVec[0] / triVecA.getMagnitude();
		float fractionOfB = cobVec[1] / triVecB.getMagnitude();
		if (fractionOfA + fractionOfB <= 1.0f) {
			return true;	
		}
	}
	return false;
}
