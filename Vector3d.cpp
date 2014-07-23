#include "Vector3d.h"
#include "Constants.h"
#include "Utils.h"
#include <iostream>
#include <assert.h>

using namespace std;

#define CLOSE_TO_ZERO 1.0E-12

const Vector3d Vector3d::xAxis(1,0,0);
const Vector3d Vector3d::yAxis(0,1,0);
const Vector3d Vector3d::zAxis(0,0,1);
const Vector3d Vector3d::zero(0,0,0);
	

ostream& operator <<(ostream& os, const Vector3d vec)
{
	return os<<" X:"<<vec.getX()<<" Y:"<<vec.getY()<<" Z:"<<vec.getZ();
}


double Vector3d::getMagnitude() const
{
	return sqrt(getSquareMagnitude());
}

double Vector3d::getSquareMagnitude() const
{
	return m_x * m_x + m_y * m_y + m_z * m_z;
}

bool Vector3d::isUnit() const			
{ 
	return (getMagnitude() == 1.0); 
}

bool Vector3d::isZero() const
{
	return (getMagnitude() == 0.0);
}

Vector Vector3d::toVector() const {
	return Vector(m_x, m_y, m_z);
}

/**
 * Should only be called if isZero() returns false.
 */
Vector3d Vector3d::getUnitVector() const
{
	assert(!isVeryCloseToZero());
	return getScaledCopy(1/getMagnitude()); 
}

float Vector3d::getScalarComponentInDir(const Vector3d &other) const
{
	if (other.isZero()) { // needs to be checked before calling getUnitVector
		return 0.0f;
	}
	return this->dotProduct(other.getUnitVector());
}


double Vector3d::dotProduct(const Vector3d &other) const
{
	return (this->m_x * other.m_x + 
			this->m_y * other.m_y +
			this->m_z * other.m_z);	
	
}

double Vector3d::distanceSquared(const Vector3d &other) const
{
	return ( ((this->m_x - other.m_x)*(this->m_x - other.m_x)) + 
			 ((this->m_y - other.m_y)*(this->m_y - other.m_y)) + 
			 ((this->m_z - other.m_z)*(this->m_z - other.m_z)) );
	
}


Vector3d Vector3d::crossProduct(const Vector3d &other) const
{
	return Vector3d(this->m_y * other.m_z - this->m_z * other.m_y,
					this->m_z * other.m_x - this->m_x * other.m_z,
					this->m_x * other.m_y - this->m_y * other.m_x);
}

Vector3d Vector3d::getScaledCopy(float value) const
{
	return Vector3d(m_x*value,m_y*value,m_z*value);
}

Vector3d Vector3d::getCopyWithMagnitude(float desiredMag) const
{
	assert(!isVeryCloseToZero());
	return getScaledCopy(desiredMag / getMagnitude());
}

// ---------
// MODIFIERS
// ---------

/**
 * Should only be called if isZero() returns false.
 */
void Vector3d::normalize()
{	
	assert(!isVeryCloseToZero());
	double mag = getMagnitude();
	m_x /= mag;
	m_y /= mag;
	m_z /= mag; 
}

void Vector3d::rotateBy(float angle, const Vector3d& axis,
	                    const Vector3d& origin)
{
	m_x -= origin.getX();
	m_y -= origin.getY();
	m_z -= origin.getZ();
	
	rotateBy(angle, axis);
	
	m_x += origin.getX();
	m_y += origin.getY();
	m_z += origin.getZ();
	
}

void Vector3d::rotateBy(float angle, const Vector3d& axis)
{
	angle = Utils::degToRad(angle);
	Vector3d axisc = axis;
	axisc.normalize();
	float t = 1 - cos(angle);
	float c = cos(angle);
	float s = sin(angle);
	float X = axisc.getX();
	float Y = axisc.getY();
	float Z = axisc.getZ();
	float a1 = t*X*X + c;
	float a2 = t*X*Y + s*Z;
	float a3 = t*X*Z - s*Y;
	float b1 = t*X*Y - s*Z;
	float b2 = t*Y*Y + c;
	float b3 = t*Y*Z + s*X;
	float c1 = t*X*Y + s*Y;
	float c2 = t*Y*Z - s*X;
	float c3 = t*Z*Z + c;
	float n_m_x = a1*m_x + a2*m_y+ a3*m_z;
	float n_m_y = b1*m_x + b2*m_y+ b3*m_z;
	float n_m_z = c1*m_x + c2*m_y+ c3*m_z;
	m_x = n_m_x;
	m_y = n_m_y;
	m_z = n_m_z;
}

// ---------------
// PRIVATE METHODS
// ---------------
void Vector3d::roundToZeroIfNeeded()
{
	if (isVeryCloseToZero()) {
		setToZero();
	}	
}

bool Vector3d::isVeryCloseToZero() const
{
	return (getSquareMagnitude() < CLOSE_TO_ZERO);
}

void Vector3d::setToZero()
{
	m_x = 0.0;
	m_y = 0.0;
	m_z = 0.0;	
}

// ------------------
// EXTERNAL OPERATORS
// ------------------

Vector3d operator+(const Vector3d &u, const Vector3d &v)
{
	return Vector3d(u.getX() + v.getX(),
	                u.getY() + v.getY(), 
	                u.getZ() + v.getZ());
}

Vector3d operator-(const Vector3d &u, const Vector3d &v)
{
	return Vector3d(u.getX() - v.getX(),
	                u.getY() - v.getY(), 
	                u.getZ() - v.getZ());
}

Vector3d operator*(const Vector3d &u, const float v)
{
	return Vector3d(u.getX() * v,
	                u.getY() * v, 
	                u.getZ() * v);
}
