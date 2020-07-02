#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <fstream>
#include <cmath>

#include "Scene.h"
#include "Camera.h"
#include "Color.h"
#include "Model.h"
#include "Rotation.h"
#include "Scaling.h"
#include "Translation.h"
#include "Triangle.h"
#include "Vec3.h"
#include "tinyxml2.h"
#include "Helpers.h"

using namespace tinyxml2;
using namespace std;

void persDiv(Vec4& v) {
	if (v.t != 0 && v.t != 1) {
		v.x /= v.t; v.y /= v.t; v.z /= v.t; v.t = 1;
	}
}

void Scene::drawLine(Vec4 v0, Vec4 v1, Camera* camera) {
	Vec4 tempV0, tempV1;
	tempV0 = v0; tempV1 = v1;
	if (isInScene(tempV0, tempV1)) {
		if (tempV0.x != v0.x || tempV1.x != v1.x) {
			double x0 = (v0.x > v1.x) ? (v1.x) : (v0.x);
			double x1 = (v0.x > v1.x) ? (v0.x) : (v1.x);
			Color c0 = (v0.x > v1.x) ? (*colorsOfVertices[tempV1.colorId - 1]) : (*colorsOfVertices[tempV0.colorId - 1]);
			Color c1 = (v0.x > v1.x) ? (*colorsOfVertices[tempV0.colorId - 1]) : (*colorsOfVertices[tempV1.colorId - 1]);
			if (tempV0.x != v0.x) {
				double alpha = (tempV0.x - x0) / (x1 - x0);
				Color* newColorV0 = new Color(c0.r*(1 - alpha) + c1.r*alpha, c0.g*(1 - alpha) + c1.g*alpha, c0.b*(1 - alpha) +  c1.b*alpha);
				colorsOfVertices.push_back(newColorV0);
				tempV0.colorId = colorsOfVertices.size();
			}
			if (tempV1.x != v1.x) {
				double alpha = (tempV1.x - x0) / (x1 - x0);
				Color* newColorV1 = new Color(c0.r*(1 - alpha) + c1.r*alpha, c0.g*(1 - alpha) + c1.g*alpha, c0.b*(1 - alpha) +  c1.b*alpha);
				colorsOfVertices.push_back(newColorV1);
				tempV1.colorId = colorsOfVertices.size();
			}
		}
		tempV0 = multiplyMatrixWithVec4(M_vp, tempV0);
		tempV1 = multiplyMatrixWithVec4(M_vp, tempV1);
		rasterization(tempV0, tempV1, camera->horRes, camera->verRes);
	}
}

bool Scene::isVisible(double den, double num, double& tE, double& tL) {
	if (den > 0) {
		double t = num/den;
		if (t > tL) {
			return false;
		} else if (t > tE) {
			tE = t;
		}
	} else if (den < 0) {
		double t = num/den;
		if (t < tE) {
			return false;
		} else if (t < tL) {
			tL = t;
		}
	} else if (num > 0) {
		return false;
	}
	return true;
}

bool Scene::isInScene(Vec4& p0, Vec4& p1) {
	double tE = 0.0;
	double tL = 1.0;
	double& x0 = p0.x;
	double& y0 = p0.y;
	double& z0 = p0.z;
	double& x1 = p1.x;
	double& y1 = p1.y;
	double& z1 = p1.z;
	double dx = (x1 - x0);
	double dy = (y1 - y0);
	double dz = (z1 - z0);
	double xmin = -1.0, xmax = 1.0;
	double ymin = -1.0, ymax = 1.0;
	double zmin = -1.0, zmax = 1.0;
	bool visible = false;
	if ((dx == 0 && xmin - x0 > 0)
	|| (dx == 0 && x0 - xmax > 0)
	|| (dy == 0 && ymin - y0 > 0)
	|| (dy == 0 && y0 - ymax > 0)
	|| (dz == 0 && zmin - z0 > 0)
	|| (dz == 0 && z0 - zmax > 0)) {
		return false;
	}
	if (isVisible(dx, xmin - x0, tE, tL)) {
		if (isVisible(-dx, x0 - xmax, tE, tL)) {
			if (isVisible(dy, ymin - y0, tE, tL)) {
				if (isVisible(-dy, y0 - ymax, tE, tL)) {
					if (isVisible(dz, zmin - z0, tE, tL)) {
						if (isVisible(-dz, z0 - zmax, tE, tL)) {
							visible = true;
							if (tL < 1) {
								x1 = x0 + dx*tL;
								y1 = y0 + dy*tL;
								z1 = z0 + dz*tL;
							}
							if (tE > 0) {
								x0 = x0 + dx*tE;
								y0 = y0 + dy*tE;
								z0 = z0 + dz*tE;
							}
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

bool Scene::isCulled(Vec4 v0, Vec4 v1, Vec4 v2, Vec3 pos) {
	Vec3 a(v0.x, v0.y, v0.z, v0.colorId);
	Vec3 b(v1.x, v1.y, v1.z, v1.colorId);
	Vec3 c(v2.x, v2.y, v2.z, v2.colorId);
	Vec3 n = crossProductVec3(subtractVec3(b, a), subtractVec3(c, a));
	Vec3 v = Vec3((a.x + b.x + c.x) / 3 - pos.x, (a.y + b.y + c.y) / 3 - pos.y, (a.z + b.z + c.z) / 3 - pos.z, 0);
	if (dotProductVec3(n, v) > 0)
		return true;
	return false;

}

double Scene::f(double x, double y, Vec4 p0, Vec4 p1) {
	return x*(p0.y - p1.y) + y*(p1.x - p0.x) + p0.x*p1.y - p1.x * p0.y;
}

void Scene::fillInside(Vec4 v0, Vec4 v1, Vec4 v2, int horRes, int verRes) {
	int x_min = round(min(min(v0.x, v1.x), v2.x));
	int x_max = round(max(max(v0.x, v1.x), v2.x));
	int y_min = round(min(min(v0.y, v1.y), v2.y));
	int y_max = round(max(max(v0.y, v1.y), v2.y));
	double f12 = f(v0.x, v0.y, v1, v2);
	double f20 = f(v1.x, v1.y, v2, v0);
	double f01 = f(v2.x, v2.y, v0, v1);
	x_min = (x_min < 0) ? (0) : (x_min);
	x_max = (x_max >= horRes) ? (horRes - 1) : (x_max);
	y_min = (y_min < 0) ? (0) : (y_min);
	y_max = (y_max >= verRes) ? (verRes - 1) : (y_max);
	Color c0 = *colorsOfVertices[v0.colorId - 1];
	Color c1 = *colorsOfVertices[v1.colorId - 1];
	Color c2 = *colorsOfVertices[v2.colorId - 1];
	for (int y = y_min; y <= y_max; y++) {
		for (int x = x_min; x <= x_max; x++) {
			double alpha = f(x, y, v1, v2) / f12;
			double beta = f(x, y, v2, v0) / f20;
			double gamma = f(x, y, v0, v1) / f01;
			if (alpha >= 0.0 && alpha <= 1.0
			 && beta >= 0.0 && beta <= 1.0
			 && gamma >= 0.0 && gamma <= 1.0) {
				double r = alpha*c0.r + beta*c1.r + gamma*c2.r;
				double g = alpha*c0.g + beta*c1.g + gamma*c2.g;
				double b = alpha*c0.b + beta*c1.b + gamma*c2.b;
				image[x][y] = Color((r > 255) ? (255) : (r),
									(g > 255) ? (255) : (g),
									(b > 255) ? (255) : (b));
			}
		}
	}
}

void Scene::rasterization(Vec4 p0, Vec4 p1, int horRes, int verRes) {
	double m = (p1.y - p0.y) / (p1.x - p0.x);
	double x0, y0, x1, y1;
	Color c0, c1;
	if (p1.x > p0.x) {
		x0 = p0.x; y0 = p0.y;
		x1 = p1.x; y1 = p1.y;
		c0 = *colorsOfVertices[p0.colorId - 1];
		c1 = *colorsOfVertices[p1.colorId - 1];
	} else {
		x0 = p1.x; y0 = p1.y;
		x1 = p0.x; y1 = p0.y;
		c0 = *colorsOfVertices[p1.colorId - 1];
		c1 = *colorsOfVertices[p0.colorId - 1];
	}
	double dN = x1 - x0;
	double dE = y0 - y1;
	if (m >= 0 && m < 1) {
		int y = round(y0); y = (y < 0) ? (0) : y;
		int d = round(2 * dE + dN);
		int _2dNE = round(2*(dN + dE));
		int _2dE = round(2*dE);
		Color c = c0;
		Color dC((c1.r - c0.r) / abs(x1 - x0), (c1.g - c0.g) / abs(x1 - x0), (c1.b - c0.b) / abs(x1 - x0));
		int s = round(x0), e = round(x1);
		s = (s < 0) ? (0) : ((s >= horRes) ? (horRes - 1) : (s));
		e = (e < 0) ? (0) : ((e >= horRes) ? (horRes - 1) : (e));
		for (int x = s; x <= e; x++) {
			image[x][y] = c;
			if (d < 0) {
				y += 1;
				d += _2dNE;
			} else {
				d += _2dE;
			}
			c.r += dC.r;
			c.g += dC.g;
			c.b += dC.b;
		}
	} else if (m >= 1) {
		int x = round(x0); x = (x < 0) ? (0) : x;
		int d = round(dE + 2 * dN);
		int _2dNE = round(2*(dN + dE));
		int _2dN = round(2*dN);
		Color c = c0;
		Color dC((c1.r - c0.r) / abs(y1 - y0), (c1.g - c0.g) / abs(y1 - y0), (c1.b - c0.b) / abs(y1 - y0));
		int s = round(y0), e = round(y1);
		s = (s < 0) ? (0) : ((s >= horRes) ? (horRes - 1) : (s));
		e = (e < 0) ? (0) : ((e >= horRes) ? (horRes - 1) : (e));
		for (int y = s; y <= e; y++) {
			image[x][y] = c;
			if (d > 0) {
				x += 1;
				d += _2dNE;
			} else {
				d += _2dN;
			}
			c.r += dC.r;
			c.g += dC.g;
			c.b += dC.b;
		}
	} else if (m <= 0 && m > -1) {
		int y = round(y0); y = (y < 0) ? (0) : y;
		int d = round(2 * dE - dN);
		int _2dmNE = round(2*(-dN + dE));
		int _2dE = round(2*dE);
		Color c = c0;
		Color dC((c1.r - c0.r) / abs(x1 - x0), (c1.g - c0.g) / abs(x1 - x0), (c1.b - c0.b) / abs(x1 - x0));
		int s = round(x0), e = round(x1);
		s = (s < 0) ? (0) : ((s >= horRes) ? (horRes - 1) : (s));
		e = (e < 0) ? (0) : ((e >= horRes) ? (horRes - 1) : (e));
		for (int x = s; x <= e; x++) {
			image[x][y] = c;
			if (d > 0) {
				y -= 1;
				d += _2dmNE;
			} else {
				d += _2dE;
			}
			c.r += dC.r;
			c.g += dC.g;
			c.b += dC.b;
		}
	} else if (m <= -1) {
		int x = round(x0); x = (x < 0) ? (0) : x;
		int d = round(dE - 2 * dN);
		int _2dmNE = round(2*(-dN + dE));
		int _2dmN = round(2*(-dN));
		Color c = c0;
		Color dC((c1.r - c0.r) / abs(y1 - y0), (c1.g - c0.g) / abs(y1 - y0), (c1.b - c0.b) / abs(y1 - y0));
		int s = round(y0), e = round(y1);
		s = (s < 0) ? (0) : ((s >= horRes) ? (horRes - 1) : (s));
		e = (e < 0) ? (0) : ((e >= horRes) ? (horRes - 1) : (e));
		for (int y = s; y >= e; y--) {
			image[x][y] = c;
			if (d < 0) {
				x += 1;
				d += _2dmNE;
			} else {
				d += _2dmN;
			}
			c.r += dC.r;
			c.g += dC.g;
			c.b += dC.b;
		}
	}
}

void Scene::constructConstantMatrices(Camera *camera){
	double m_cam[4][4] = {{(camera->u).x, (camera->u).y, (camera->u).z, (-1.0) * ((camera->u).x * (camera->pos).x + (camera->u).y * (camera->pos).y + (camera->u).z * (camera->pos).z)},
					  	  {(camera->v).x, (camera->v).y, (camera->v).z, (-1.0) * ((camera->v).x * (camera->pos).x + (camera->v).y * (camera->pos).y + (camera->v).z * (camera->pos).z)},
					      {(camera->w).x, (camera->w).y, (camera->w).z, (-1.0) * ((camera->w).x * (camera->pos).x + (camera->w).y * (camera->pos).y + (camera->w).z * (camera->pos).z)},
					      {0, 0, 0, 1}};//Camera Transformation
	M_cam = Matrix4(m_cam);
	if (projectionType == 0) {
		double m_orth[4][4] = {{2.0 / (camera->right - camera->left), 0, 0, (-1.0) * ((camera->right + camera->left) / (camera->right - camera->left))},
						   	   {0, 2.0 / (camera->top - camera->bottom), 0, (-1.0) * ((camera->top + camera->bottom) / (camera->top - camera->bottom))},
						       {0, 0, (-1.0) * (2.0 / (camera->far - camera->near)), (-1.0) * ((camera->far + camera->near) / (camera->far - camera->near))},
						       {0, 0, 0, 1}};//Orthographic Projection
		M_orth_or_per = Matrix4(m_orth);
	} else {
		double m_per[4][4] = {{2.0 * camera->near / (camera->right - camera->left), 0, ((camera->right + camera->left) / (camera->right - camera->left)), 0},
						   	  {0, 2.0 * camera->near / (camera->top - camera->bottom), ((camera->top + camera->bottom) / (camera->top - camera->bottom)), 0},
						      {0, 0, (-1.0) * ((camera->far + camera->near) / (camera->far - camera->near)), (-2.0) * camera->far * camera->near / (camera->far - camera->near)},
						      {0, 0, -1, 0}};//Orthographic Projection
		M_orth_or_per = Matrix4(m_per);
	}
	double m_vp[4][4] = {{(double)camera->horRes / 2.0, 0, 0, (double)(camera->horRes - 1) / 2.0},
						 {0, (double)camera->verRes / 2.0, 0, (double)(camera->verRes - 1) / 2.0},
						 {0, 0, 0.5, 0.5},
						 {0, 0, 0, 0}};
	M_vp = Matrix4(m_vp);
	return;
}

/*
	Transformations, clipping, culling, rasterization are done here.
	You can define helper functions inside Scene class implementation.
*/
void Scene::forwardRenderingPipeline(Camera *camera) {
	// TODO: Implement this function.
	// Do not forget perspective divide!!!!
	// Construct constant matrices
	constructConstantMatrices(camera);
	// for each model
	for (Model* model: models) {
	//	 for each trianle
		for (Triangle& triangle: model->triangles) {
			vector< Vec4> foundVertices;
			Vec4 v0 = Vec4(vertices[triangle.vertexIds[0] - 1]->x,
						   vertices[triangle.vertexIds[0] - 1]->y,
						   vertices[triangle.vertexIds[0] - 1]->z, 1, triangle.vertexIds[0]);
			Vec4 v1 = Vec4(vertices[triangle.vertexIds[1] - 1]->x,
						   vertices[triangle.vertexIds[1] - 1]->y,
						   vertices[triangle.vertexIds[1] - 1]->z, 1, triangle.vertexIds[1]);
			Vec4 v2 = Vec4(vertices[triangle.vertexIds[2] - 1]->x,
						   vertices[triangle.vertexIds[2] - 1]->y,
						   vertices[triangle.vertexIds[2] - 1]->z, 1, triangle.vertexIds[2]);
			// Start modeling transformations
			for (int i = 0; i < model->numberOfTransformations; i++) {
				if (model->transformationTypes[i] == 't') {
					Translation* t = translations[(model->transformationIds)[i] - 1];
					double m[4][4] = {{1, 0, 0, t->tx},
									  {0, 1, 0, t->ty},
									  {0, 0, 1, t->tz},
									  {0, 0, 0, 1}};
					v0 = multiplyMatrixWithVec4(Matrix4(m), v0);
					v1 = multiplyMatrixWithVec4(Matrix4(m), v1);
					v2 = multiplyMatrixWithVec4(Matrix4(m), v2);
				} else if (model->transformationTypes[i] == 's') {
					Scaling* s = scalings[(model->transformationIds)[i] - 1];
					double m[4][4] = {{s->sx, 0, 0, 0},
									  {0, s->sy, 0, 0},
									  {0, 0, s->sz, 0},
									  {0, 0, 0, 1}};
					v0 = multiplyMatrixWithVec4(Matrix4(m), v0);
					v1 = multiplyMatrixWithVec4(Matrix4(m), v1);
					v2 = multiplyMatrixWithVec4(Matrix4(m), v2);
				} else if (model->transformationTypes[i] == 'r') {
					Rotation* r = rotations[(model->transformationIds)[i] - 1];
					Vec3 u = normalizeVec3(Vec3(r->ux, r->uy, r->uz, 1));
					Vec3 v;
					if (abs(u.x) <= abs(u.y) && abs(u.x) <= abs(u.z)) {
						v.x = 0; v.y = -u.z; v.z = u.y;
					} else if (abs(u.y) <= abs(u.x) && abs(u.y) <= abs(u.z)) {
						v.x = -u.z; v.y = 0; v.z = u.x;
					} else if (abs(u.z) <= abs(u.x) && abs(u.z) <= abs(u.y)) {
						v.x = -u.y; v.y = u.x; v.z = 0;
					}
					v = normalizeVec3(v);
					Vec3 w = normalizeVec3(crossProductVec3(u, v));
					double m[4][4] = {{u.x, u.y, u.z, 0},
									  {v.x, v.y, v.z, 0},
									  {w.x, w.y, w.z, 0},
									  {0, 0, 0, 1}};
					double m_inverse[4][4] = {{u.x, v.x, w.x, 0},
									  		  {u.y, v.y, w.y, 0},
									  		  {u.z, v.z, w.z, 0},
									  		  {0, 0, 0, 1}};
					double rad = r->angle * M_PI / 180.0;
					double r_x[4][4] = {{1, 0, 0, 0},
									    {0, cos(rad), -sin(rad), 0},
									    {0, sin(rad), cos(rad), 0},
									    {0, 0, 0, 1}};
					v0 = multiplyMatrixWithVec4(Matrix4(m), v0);
					v0 = multiplyMatrixWithVec4(Matrix4(r_x), v0);
					v0 = multiplyMatrixWithVec4(Matrix4(m_inverse), v0);
					v1 = multiplyMatrixWithVec4(Matrix4(m), v1);
					v1 = multiplyMatrixWithVec4(Matrix4(r_x), v1);
					v1 = multiplyMatrixWithVec4(Matrix4(m_inverse), v1);
					v2 = multiplyMatrixWithVec4(Matrix4(m), v2);
					v2 = multiplyMatrixWithVec4(Matrix4(r_x), v2);
					v2 = multiplyMatrixWithVec4(Matrix4(m_inverse), v2);
				}
			}
			// End modeling transformations
			// Backface culling
			if (cullingEnabled == 1 && isCulled(v0, v1, v2, camera->pos)) {
				continue;
			}
			// Start viewing transformations
			v0 = multiplyMatrixWithVec4(M_cam, v0);
			v0 = multiplyMatrixWithVec4(M_orth_or_per, v0);
			v1 = multiplyMatrixWithVec4(M_cam, v1);
			v1 = multiplyMatrixWithVec4(M_orth_or_per, v1);
			v2 = multiplyMatrixWithVec4(M_cam, v2);
			v2 = multiplyMatrixWithVec4(M_orth_or_per, v2);
			// End viewing transformations
			// Perspective divide
			persDiv(v0); persDiv(v1); persDiv(v2);
			if (model->type == 1) {
				v0 = multiplyMatrixWithVec4(M_vp, v0);
				v1 = multiplyMatrixWithVec4(M_vp, v1);
				v2 = multiplyMatrixWithVec4(M_vp, v2);
				fillInside(v0, v1, v2, camera->horRes, camera->verRes);
			} else {
				drawLine(v0, v1, camera);
				drawLine(v1, v2, camera);
				drawLine(v2, v0, camera);
			}
		}
	}
}

/*
	Parses XML file
*/
Scene::Scene(const char *xmlPath)
{
	const char *str;
	XMLDocument xmlDoc;
	XMLElement *pElement;

	xmlDoc.LoadFile(xmlPath);

	XMLNode *pRoot = xmlDoc.FirstChild();

	// read background color
	pElement = pRoot->FirstChildElement("BackgroundColor");
	str = pElement->GetText();
	sscanf(str, "%lf %lf %lf", &backgroundColor.r, &backgroundColor.g, &backgroundColor.b);

	// read culling
	pElement = pRoot->FirstChildElement("Culling");
	if (pElement != NULL)
		pElement->QueryBoolText(&cullingEnabled);

	// read projection type
	pElement = pRoot->FirstChildElement("ProjectionType");
	if (pElement != NULL)
		pElement->QueryIntText(&projectionType);

	// read cameras
	pElement = pRoot->FirstChildElement("Cameras");
	XMLElement *pCamera = pElement->FirstChildElement("Camera");
	XMLElement *camElement;
	while (pCamera != NULL)
	{
		Camera *cam = new Camera();

		pCamera->QueryIntAttribute("id", &cam->cameraId);

		camElement = pCamera->FirstChildElement("Position");
		str = camElement->GetText();
		sscanf(str, "%lf %lf %lf", &cam->pos.x, &cam->pos.y, &cam->pos.z);

		camElement = pCamera->FirstChildElement("Gaze");
		str = camElement->GetText();
		sscanf(str, "%lf %lf %lf", &cam->gaze.x, &cam->gaze.y, &cam->gaze.z);

		camElement = pCamera->FirstChildElement("Up");
		str = camElement->GetText();
		sscanf(str, "%lf %lf %lf", &cam->v.x, &cam->v.y, &cam->v.z);

		cam->gaze = normalizeVec3(cam->gaze);
		cam->u = crossProductVec3(cam->gaze, cam->v);
		cam->u = normalizeVec3(cam->u);

		cam->w = inverseVec3(cam->gaze);
		cam->v = crossProductVec3(cam->u, cam->gaze);
		cam->v = normalizeVec3(cam->v);

		camElement = pCamera->FirstChildElement("ImagePlane");
		str = camElement->GetText();
		sscanf(str, "%lf %lf %lf %lf %lf %lf %d %d",
			   &cam->left, &cam->right, &cam->bottom, &cam->top,
			   &cam->near, &cam->far, &cam->horRes, &cam->verRes);

		camElement = pCamera->FirstChildElement("OutputName");
		str = camElement->GetText();
		cam->outputFileName = string(str);

		cameras.push_back(cam);

		pCamera = pCamera->NextSiblingElement("Camera");
	}

	// read vertices
	pElement = pRoot->FirstChildElement("Vertices");
	XMLElement *pVertex = pElement->FirstChildElement("Vertex");
	int vertexId = 1;

	while (pVertex != NULL)
	{
		Vec3 *vertex = new Vec3();
		Color *color = new Color();

		vertex->colorId = vertexId;

		str = pVertex->Attribute("position");
		sscanf(str, "%lf %lf %lf", &vertex->x, &vertex->y, &vertex->z);

		str = pVertex->Attribute("color");
		sscanf(str, "%lf %lf %lf", &color->r, &color->g, &color->b);

		vertices.push_back(vertex);
		colorsOfVertices.push_back(color);

		pVertex = pVertex->NextSiblingElement("Vertex");

		vertexId++;
	}

	// read translations
	pElement = pRoot->FirstChildElement("Translations");
	XMLElement *pTranslation = pElement->FirstChildElement("Translation");
	while (pTranslation != NULL)
	{
		Translation *translation = new Translation();

		pTranslation->QueryIntAttribute("id", &translation->translationId);

		str = pTranslation->Attribute("value");
		sscanf(str, "%lf %lf %lf", &translation->tx, &translation->ty, &translation->tz);

		translations.push_back(translation);

		pTranslation = pTranslation->NextSiblingElement("Translation");
	}

	// read scalings
	pElement = pRoot->FirstChildElement("Scalings");
	XMLElement *pScaling = pElement->FirstChildElement("Scaling");
	while (pScaling != NULL)
	{
		Scaling *scaling = new Scaling();

		pScaling->QueryIntAttribute("id", &scaling->scalingId);
		str = pScaling->Attribute("value");
		sscanf(str, "%lf %lf %lf", &scaling->sx, &scaling->sy, &scaling->sz);

		scalings.push_back(scaling);

		pScaling = pScaling->NextSiblingElement("Scaling");
	}

	// read rotations
	pElement = pRoot->FirstChildElement("Rotations");
	XMLElement *pRotation = pElement->FirstChildElement("Rotation");
	while (pRotation != NULL)
	{
		Rotation *rotation = new Rotation();

		pRotation->QueryIntAttribute("id", &rotation->rotationId);
		str = pRotation->Attribute("value");
		sscanf(str, "%lf %lf %lf %lf", &rotation->angle, &rotation->ux, &rotation->uy, &rotation->uz);

		rotations.push_back(rotation);

		pRotation = pRotation->NextSiblingElement("Rotation");
	}

	// read models
	pElement = pRoot->FirstChildElement("Models");

	XMLElement *pModel = pElement->FirstChildElement("Model");
	XMLElement *modelElement;
	while (pModel != NULL)
	{
		Model *model = new Model();

		pModel->QueryIntAttribute("id", &model->modelId);
		pModel->QueryIntAttribute("type", &model->type);

		// read model transformations
		XMLElement *pTransformations = pModel->FirstChildElement("Transformations");
		XMLElement *pTransformation = pTransformations->FirstChildElement("Transformation");

		pTransformations->QueryIntAttribute("count", &model->numberOfTransformations);

		while (pTransformation != NULL)
		{
			char transformationType;
			int transformationId;

			str = pTransformation->GetText();
			sscanf(str, "%c %d", &transformationType, &transformationId);

			model->transformationTypes.push_back(transformationType);
			model->transformationIds.push_back(transformationId);

			pTransformation = pTransformation->NextSiblingElement("Transformation");
		}

		// read model triangles
		XMLElement *pTriangles = pModel->FirstChildElement("Triangles");
		XMLElement *pTriangle = pTriangles->FirstChildElement("Triangle");

		pTriangles->QueryIntAttribute("count", &model->numberOfTriangles);

		while (pTriangle != NULL)
		{
			int v1, v2, v3;

			str = pTriangle->GetText();
			sscanf(str, "%d %d %d", &v1, &v2, &v3);

			model->triangles.push_back(Triangle(v1, v2, v3));

			pTriangle = pTriangle->NextSiblingElement("Triangle");
		}

		models.push_back(model);

		pModel = pModel->NextSiblingElement("Model");
	}
}

/*
	Initializes image with background color
*/
void Scene::initializeImage(Camera *camera)
{
	if (this->image.empty())
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			vector<Color> rowOfColors;

			for (int j = 0; j < camera->verRes; j++)
			{
				rowOfColors.push_back(this->backgroundColor);
			}

			this->image.push_back(rowOfColors);
		}
	}
	// if image is filled before, just change color rgb values with the background color
	else
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			for (int j = 0; j < camera->verRes; j++)
			{
				this->image[i][j].r = this->backgroundColor.r;
				this->image[i][j].g = this->backgroundColor.g;
				this->image[i][j].b = this->backgroundColor.b;
			}
		}
	}
}

/*
	If given value is less than 0, converts value to 0.
	If given value is more than 255, converts value to 255.
	Otherwise returns value itself.
*/
int Scene::makeBetweenZeroAnd255(double value)
{
	if (value >= 255.0)
		return 255;
	if (value <= 0.0)
		return 0;
	return (int)(value);
}

/*
	Writes contents of image (Color**) into a PPM file.
*/
void Scene::writeImageToPPMFile(Camera *camera)
{
	ofstream fout;

	fout.open(camera->outputFileName.c_str());

	fout << "P3" << endl;
	fout << "# " << camera->outputFileName << endl;
	fout << camera->horRes << " " << camera->verRes << endl;
	fout << "255" << endl;

	for (int j = camera->verRes - 1; j >= 0; j--)
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			fout << makeBetweenZeroAnd255(this->image[i][j].r) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].g) << " "
				 << makeBetweenZeroAnd255(this->image[i][j].b) << " ";
		}
		fout << endl;
	}
	fout.close();
}

/*
	Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
	os_type == 1 		-> Ubuntu
	os_type == 2 		-> Windows
	os_type == other	-> No conversion
*/
void Scene::convertPPMToPNG(string ppmFileName, int osType)
{
	string command;

	// call command on Ubuntu
	if (osType == 1)
	{
		command = "convert " + ppmFileName + " " + ppmFileName + ".png";
		system(command.c_str());
	}

	// call command on Windows
	else if (osType == 2)
	{
		command = "magick convert " + ppmFileName + " " + ppmFileName + ".png";
		system(command.c_str());
	}

	// default action - don't do conversion
	else
	{
	}
}