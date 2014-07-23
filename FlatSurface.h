#ifndef FLATSURFACE_H_
#define FLATSURFACE_H_

#include "Vector3d.h"
#include "LineSegment.h"
#include "MaterialProperties.h"
#include "sdlheaders.h"
#include <math/matrix4.h>
#include <vector>
using namespace std;

/**
 * Superclass to group together common functionality + data between
 * triangle and parallelogram
 */
class FlatSurface
{
  public:
  
	typedef enum {
		TRIANGLE,
		PARALLELOGRAM,
		NUM_FLAT_SURFACE_TYPES
	} FlatSurfaceType;

	FlatSurface(const Vector3d &origin, const Vector3d &vecA, 
                  const Vector3d &vecB);    
    
	FlatSurface(const FlatSurface &other);
	virtual ~FlatSurface()                                   { }


	const void reInitialize(FlatSurface &other);
	
    const Vector3d &getNormal() const;
    const Vector3d &getOrigin() const;
	const Vector3d &getVecA()   const;
	const Vector3d &getVecB()   const;
    
    LineSegment getEdge(unsigned index) const;    
    virtual Vector3d getVertex(unsigned index) const         { return Vector3d::zero; }
    virtual unsigned getNumEdges() const                     { return 0; }
    virtual unsigned getNumVertices() const                  { return 0; }
    virtual bool isInBounds(const Vector3d &position) const  { return false; }
    virtual void drawMe() const                              { }
    virtual FlatSurfaceType getType() const           { return NUM_FLAT_SURFACE_TYPES; }
	
	void setTopTextureRepetition(float x, float y);
    double getDistanceBelow(const Vector3d &position) const;
   	void rotate(float rot_angle, const Vector3d &rot_axis, 
                const Vector3d &rot_origin); 
	
	void setTexture(GLuint texture);
	void setTextureCoord(int vertexIndex, const Vector3d &coord);
	void setBumpMapping(bool isBumpMappping);
		
	// Returns NUM_FLAT_SURFACE_TYPES if there's no flat surface with the given name
	static FlatSurfaceType getTypeForName(const string &surfaceName);
	static string getNameForType(FlatSurfaceType type);
	
	
  protected:
  	Vector3d m_origin;
	Vector3d m_vecA;   // to define one set of parallel sides
	Vector3d m_vecB;   // to define other set of parallel sides
	
	// Texture Info
	vector<Vector3d> m_texCoords;
	GLuint m_texture;
	bool m_isBumpMapping;
	
	// IMPORTANT: Update normal, cobMatrix, and edges whenever vecA or vecB is changed!!!
	Vector3d m_normal; 
	Matrix m_cobMatrix;
	vector<LineSegment> m_edges;
	
	float topTextureRepetitionX;
	float topTextureRepetitionY;
	
	static const string s_flatSurfaceNames[];
		
	void updateNormal();
	void updateCobMatrix();
	virtual void updateEdges(); // virtual because it's called in constructor and depends on subclass' function getVertex()

	Vector3d getClosestPointTo(const Vector3d &pt) const;
    static bool isPointInTriangleBounds(const Vector3d &point, const Vector3d &triOrigin,
                                        const Vector3d &triVecA, const Vector3d &triVecB,
                                        const Matrix &cobMatrix, bool flipSign);
};

#endif /*FLATSURFACE_H_*/
