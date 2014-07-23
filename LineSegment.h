#ifndef LINESEGMENT_H_
#define LINESEGMENT_H_

#include "Vector3d.h"
#include "Shape.h"

class LineSegment
{
 public:
	LineSegment(const Vector3d &vertexA, const Vector3d &vertexB);
	~LineSegment();
	
	double getDistanceToShape(const Shape* shape) const;
	double isPointInBounds(const Vector3d &point) const;
	Vector3d getNormalInDirectionOf(const Vector3d &pt) const;
	
 private:
 	Vector3d m_vertexA;
 	Vector3d m_vertexB;
 	
 	static double isObtuseAngle(const Vector3d &a, const Vector3d &b, const Vector3d &c); 	
};

#endif /*LINESEGMENT_H_*/
