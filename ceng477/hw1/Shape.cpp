#include "Shape.h"
#include "Scene.h"
#include <cstdio>
#include <cmath>

Shape::Shape(void) {}

Shape::Shape(int id, int matIndex)
    : id(id), matIndex(matIndex) {}

Sphere::Sphere(void) {}

/* Constructor for sphere. You will implement this. */
Sphere::Sphere(int id, int matIndex, int cIndex, float R)
    : Shape(id, matIndex), cIndex(cIndex), R(R) {}

/* Sphere-ray intersection routine. You will implement this. 
Note that ReturnVal structure should hold the information related to the intersection point, e.g., coordinate of that point, normal at that point etc. 
You should to declare the variables in ReturnVal structure you think you will need. It is in defs.h file. */
ReturnVal Sphere::intersect(const Ray & ray) const {
	ReturnVal result;
	Vector3f d = ray.direction;
	Vector3f o = ray.origin;
	Vector3f c = pScene -> vertices[cIndex - 1];
	float delta = ((d.dot(o - c) * d.dot(o - c)) - d.dot(d) * ((o - c).dot(o - c) - (R * R)));
	float t;
	if (delta >= 0 && delta <= pScene -> intTestEps) {
		t = ((d * (-1)).dot(o - c)) / (d.dot(d));
		if (t < 0)
			return result;
	} else if (delta > pScene -> intTestEps) {
		float t_1 = ((d * (-1)).dot(o - c) + sqrt(delta)) / (d.dot(d));
		float t_2 = ((d * (-1)).dot(o - c) - sqrt(delta)) / (d.dot(d));
		if (t_1 > 0 && t_2 > 0)
			t = (t_1 > t_2) ? t_2 : t_1;
		else if (t_1 > 0)
			t = t_1;
		else if (t_2 > 0)
			t = t_2;
		else
			return result;
	} else {
		return result;
	}
	result.intersecting = true;
	result.matIndex = matIndex;
	result.point = ray.getPoint(t);
	result.normalVector = (result.point - c).normalize();
	result.t = t;
	result.id = id;
	return result;
}

Triangle::Triangle(void) {}

/* Constructor for triangle. You will implement this. */
Triangle::Triangle(int id, int matIndex, int p1Index, int p2Index, int p3Index)
    : Shape(id, matIndex), p1Index(p1Index), p2Index(p2Index), p3Index(p3Index) {}

/* Triangle-ray intersection routine. You will implement this. 
Note that ReturnVal structure should hold the information related to the intersection point, e.g., coordinate of that point, normal at that point etc. 
You should to declare the variables in ReturnVal structure you think you will need. It is in defs.h file. */
ReturnVal Triangle::intersect(const Ray & ray) const {
	ReturnVal result;
	Vector3f a = pScene -> vertices[p1Index - 1];
	Vector3f b = pScene -> vertices[p2Index - 1];
	Vector3f c = pScene -> vertices[p3Index - 1];
	Vector3f A[3] = {a - b, a - c, ray.direction};
	Vector3f beta_cramer_matrix[3] = {a - ray.origin, a - c, ray.direction};
	Vector3f gamma_cramer_matrix[3] = {a - b, a - ray.origin, ray.direction};
	Vector3f t_cramer_matrix[3] = {a - b, a - c, a - ray.origin};
	float det_A = A[0].det(A[1], A[2]);
	float det_t_cramer_matrix = t_cramer_matrix[0].det(t_cramer_matrix[1], t_cramer_matrix[2]);
	float t = det_t_cramer_matrix / det_A;
	if (t < 0)
		return result;
	float det_gamma_cramer_matrix = gamma_cramer_matrix[0].det(gamma_cramer_matrix[1], gamma_cramer_matrix[2]);
	float gamma = det_gamma_cramer_matrix / det_A;
	if (gamma < 0 || gamma > 1)
		return result;
	float det_beta_cramer_matrix = beta_cramer_matrix[0].det(beta_cramer_matrix[1], beta_cramer_matrix[2]);
	float beta = det_beta_cramer_matrix / det_A;
	if (beta < 0 || beta > (1 - gamma))
		return result;
	result.intersecting = true;
	result.matIndex = matIndex;
	result.point = ray.getPoint(t);
	result.normalVector = ((b - a).cross(c - a)).normalize();
	if (result.normalVector.dot(ray.direction) > 0)
		result.normalVector = result.normalVector * (-1);
	result.t = t;
	result.id = id;
	return result;
}

Mesh::Mesh() {}

/* Constructor for mesh. You will implement this. */
Mesh::Mesh(int id, int matIndex, const vector<Triangle>& faces)
    : Shape(id, matIndex), faces(faces) {}

/* Mesh-ray intersection routine. You will implement this. 
Note that ReturnVal structure should hold the information related to the intersection point, e.g., coordinate of that point, normal at that point etc. 
You should to declare the variables in ReturnVal structure you think you will need. It is in defs.h file. */
ReturnVal Mesh::intersect(const Ray & ray) const {
	ReturnVal result;
	for (Triangle face: faces) {
		ReturnVal temp = face.intersect(ray);
		if (temp.intersecting && result.t > temp.t) {
			result = temp;
		}
	}
	return result;
}

