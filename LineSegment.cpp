#include "LineSegment.h"
#include "Shape.h"
#include "Utils.h"
#include <assert.h>

LineSegment::LineSegment(const Vector3d &vertexA, const Vector3d &vertexB)
	: m_vertexA(vertexA), m_vertexB(vertexB)
{
	//assert(vertexA != vertexB); // DWH: Need to have operator!= for Vector3d?
}

LineSegment::~LineSegment()
{
}

// From Wolfram MathWorld: Distance from a point to a line
// http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html
// 
// Shortest dist. from point x0 to line connecting x1 to x2 is:
// ||(x2-x1) x (x1 - x0)|| / ||x2 - x1||
// 
// If we use a normalized x2-x1 vector, then we don't need to divide by
// its magnitude, because its magnitude is 1.
double LineSegment::getDistanceToShape(const Shape* shape) const
{
	const Vector3d &shiftedPt = shape->getPosition() - m_vertexA;	
	Vector3d lineVec = (m_vertexB - m_vertexA).getUnitVector();
	Vector3d temp = shiftedPt.crossProduct(lineVec);	
	return temp.getMagnitude();	
}

/**
 * Checks if a point (either on the line segment or not) is in-bounds
 * of the line segment. (i.e. checks if its projection onto the line 
 * falls within the segment.)
 */
double LineSegment::isPointInBounds(const Vector3d &point) const
{
	return(Utils::isAcuteAngle(point, m_vertexA, m_vertexB) &&
		   Utils::isAcuteAngle(point, m_vertexB, m_vertexA));
}

Vector3d LineSegment::getNormalInDirectionOf(const Vector3d &pt) const
{
	// TODO: Handle these cases
	//assert(!(pt == m_vertexA));
	//assert(!(pt == m_vertexB));
	
	Vector3d lineVec = m_vertexB - m_vertexA;
	Vector3d temp = lineVec.crossProduct(pt - m_vertexA);	
	Vector3d normal = lineVec.crossProduct(temp); // Normal to line in the plane of line and pt
	return normal;	
}
