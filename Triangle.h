#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "FlatSurface.h"
#include "Vector3d.h"
#include "Shape.h"
#include "LineSegment.h"
#include <vector> 
using namespace std;

class Triangle : public FlatSurface 
{
 public:
   /**
    * Constructor
    * ----------
    * origin: some triangle vertex
    * vecA: vector that defines one of the edges from the "origin" vertex
    * vecB: vector that defines the other edge from the "origin" vertex
    * 
    * Ordering of vecA and vecB matters for sign of the normal vector.
    * (Normal points in direction of vecA X vecB)
    */ 
    Triangle(const Vector3d &origin, const Vector3d &vecA, 
             const Vector3d &vecB);
    
    Triangle(const Triangle &other);
    
    Vector3d getVertex(unsigned index) const;
    
    // Though it might be tempting, these need to NOT be static, 
    // so they can ovverride the superclass's methods
    unsigned getNumEdges() const;
    unsigned getNumVertices() const;
    
    bool isInBounds(const Vector3d &point) const;
    void drawMe() const;
    FlatSurfaceType getType() const;
    
  protected:
	void updateEdges();
};

#endif /*TRIANGLE_H*/
