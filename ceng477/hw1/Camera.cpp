#include "Camera.h"

Camera::Camera(int id,                      // Id of the camera
               const char* imageName,       // Name of the output PPM file 
               const Vector3f& pos,         // Camera position
               const Vector3f& gaze,        // Camera gaze direction
               const Vector3f& up,          // Camera up direction
               const ImagePlane& imgPlane)  // Image plane parameters
	: id(id), pos(pos), gaze(gaze), up(up) {
	strncpy(this -> imageName, imageName, strlen(imageName) + 1);
	this -> imgPlane = imgPlane;
}

/* Takes coordinate of an image pixel as row and col, and
 * returns the ray going through that pixel. 
 */
Ray Camera::getPrimaryRay(int col, int row) const {
	Vector3f v = up.normalize();
	Vector3f w = (gaze * (-1)).normalize();
	Vector3f u = (v.cross(w)).normalize();
	Vector3f e = pos;
	Vector3f q = e + w * (-1) * imgPlane.distance + u * imgPlane.left + v * imgPlane.top;
	float s_u = ((float)col + 0.5) * ((float)(imgPlane.right - imgPlane.left) / (float)(imgPlane.nx));
	float s_v = ((float)row + 0.5) * ((float)(imgPlane.top - imgPlane.bottom) / (float)(imgPlane.ny));
	Vector3f s = q + u * s_u - v * s_v;
	Vector3f d = s - e;
	return Ray(e, d);
}

