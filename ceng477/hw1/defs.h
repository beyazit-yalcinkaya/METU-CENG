#ifndef _DEFS_H_
#define _DEFS_H_

#include <cmath>
#include <climits>

class Scene;

/* 3 dimensional vector holding floating point numbers.
Used for both coordinates and color. 
Use x, y, z for coordinate computations, and use r, g, b for color computations. 
Note that you do not have to use this one if you use any vector computation library like Eigen. */
typedef struct Vector3f
{
	union 
	{
		float x;
		float r;
	};
	union
	{
		float y;
		float g;
	};
	union
	{
		float z;
		float b;
	};
    Vector3f() : x(0), y(0), z(0) {}
    Vector3f(float x, float y, float z) : x(x), y(y), z(z) {}
    Vector3f operator+(Vector3f a) const {
        return Vector3f(x + a.x, y + a.y, z + a.z);
    }
    Vector3f operator-(Vector3f a) const {
        return Vector3f(x - a.x, y - a.y, z - a.z);
    }
    Vector3f operator*(float c) const {
        return Vector3f(x == 0 ? 0 : c * x, y == 0 ? 0 : c * y, z == 0 ? 0 : c * z);
    }
    Vector3f operator/(float c) const {
        return Vector3f(x == 0 ? 0 : x / c, y == 0 ? 0 : y / c, z == 0 ? 0 : z / c);
    }
    Vector3f operator*(Vector3f a) const {
        return Vector3f(x * a.x, y * a.y, z * a.z);
    }
    float dot(Vector3f a) const {
        return x * a.x + y * a.y + z * a.z;
    }
    float norm() const {
    	return sqrt(dot(*this));
    }
    Vector3f normalize() const {
        return (*this) * (1 / norm());
    }
    Vector3f cross(Vector3f a) const {
        return Vector3f(y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x);
    }
    float det(Vector3f b, Vector3f c) const {
        return x * (b.y * c.z - c.y * b.z) + y * (c.x * b.z - b.x * c.z) + z * (b.x * c.y - b.y * c.x);
    }
} Vector3f;

/* Structure to hold return value from ray intersection routine. 
This should hold information related to the intersection point, 
for example, coordinate of the intersection point, surface normal at the intersection point etc. 
Think about the variables you will need for this purpose and declare them here inside of this structure. */
typedef struct ReturnVal
{
    /***********************************************
     *                                             *
     * TODO: Implement this structure              *
     *                                             *
     ***********************************************
     */
    float t;
    int id;
    bool intersecting;
    int matIndex;
    Vector3f normalVector;
    Vector3f point;
    ReturnVal() : t(INT_MAX), id(0), intersecting(false), matIndex(0) {
        normalVector.x = 0;
        normalVector.y = 0;
        normalVector.z = 0;
        point.x = 0;
        point.y = 0;
        point.z = 0;
    }
} ReturnVal;

//
// The global variable through which you can access the scene data
//
extern Scene* pScene;

#endif
