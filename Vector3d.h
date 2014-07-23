#ifndef VECTOR_3D_H_
#define VECTOR_3D_H_

#include <math.h>
#include <math/vector3.h>
#include <iostream>

using namespace std;

class Vector3d {

	friend ostream& operator<<(ostream& os, const Vector3d vec);

public:
	static const Vector3d xAxis;
	static const Vector3d yAxis;
	static const Vector3d zAxis;
	static const Vector3d zero;


    Vector3d() :
    	m_x(0), m_y(0), m_z(0) 
    	{}
    Vector3d(double x, double y, double z) : 
    	m_x(x), m_y(y), m_z(z) 
    	{ roundToZeroIfNeeded(); }
    

	bool operator=(const Vector3d &other)  {
		m_x = other.getX();
		m_y = other.getY();
		m_z = other.getZ();
		return true;
	}
	
	bool operator==(const Vector3d &other)  {
		return (m_x == other.getX() &&
		m_y == other.getY() &&
		m_z == other.getZ());
	}


	Vector toVector() const;

	Vector3d getScaledCopy(float value) const;
	Vector3d getCopyWithMagnitude(float desiredMag) const;

    // Modifiers
    void setX(double x)   { m_x = x;  roundToZeroIfNeeded(); }
    void setY(double y)   { m_y = y;  roundToZeroIfNeeded(); }
    void setZ(double z)   { m_z = z;  roundToZeroIfNeeded(); }

    void incX(double x)   { m_x += x;  roundToZeroIfNeeded(); }
    void incY(double y)   { m_y += y;  roundToZeroIfNeeded(); }
    void incZ(double z)   { m_z += z;  roundToZeroIfNeeded(); }
    
    void decX(double x)   { m_x -= x;  roundToZeroIfNeeded(); }
    void decY(double y)   { m_y -= y;  roundToZeroIfNeeded(); }
    void decZ(double z)   { m_z -= z;  roundToZeroIfNeeded(); }
    
    void normalize();
	void rotateBy(float angle, const Vector3d& axis);
	void rotateBy(float angle, const Vector3d& axis,
	              const Vector3d& origin);
	
    
    // Accessors
    double getX() const   { return m_x; }
    double getY() const   { return m_y; }
    double getZ() const   { return m_z; }
    
    double getMagnitude() const;
    double getSquareMagnitude() const;    
    bool isZero() const;
    bool isUnit() const;
    Vector3d getUnitVector() const;  // returns a unit vector in the same direction 
	float getScalarComponentInDir(const Vector3d &other) const;
	
    double dotProduct(const Vector3d &other) const;
    Vector3d crossProduct(const Vector3d &other) const;
	
	double distanceSquared(const Vector3d &other) const;

  private:
	double m_x;
	double m_y;
	double m_z;
	

	void roundToZeroIfNeeded();
	bool isVeryCloseToZero() const;	
	void setToZero();
};

Vector3d operator+(const Vector3d &u, const Vector3d &v);
Vector3d operator-(const Vector3d &u, const Vector3d &v);
Vector3d operator*(const Vector3d &u, const float v);

#endif /*VEC_H_*/
