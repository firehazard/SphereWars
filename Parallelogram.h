#ifndef PARALLELOGRAM_H
#define PARALLELOGRAM_H

#include "FlatSurface.h"
#include "Vector3d.h"
#include "Shape.h"
#include "LineSegment.h"
#include <vector> 
using namespace std;

class Parallelogram : public FlatSurface 
{
 public:
   /**
    * Constructor
    * ----------
    * origin: Upper-left corner
    * vecA: vector that defines one set of parallel sides
    * vecB: vector that defines the other set of parallel sides
    * 
    * Ordering of vecA and vecB matters for sign of the normal vector.
    * (Normal points is vecA X vecB)
    */ 
    Parallelogram(const Vector3d &origin, const Vector3d &vecA, 
                  const Vector3d &vecB);    
    
	Parallelogram(const Parallelogram &other);
   
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

#endif /*PARALLELOGRAM_H*/
