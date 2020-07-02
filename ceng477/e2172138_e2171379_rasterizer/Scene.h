#ifndef _SCENE_H_
#define _SCENE_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "Camera.h"
#include "Color.h"
#include "Model.h"
#include "Rotation.h"
#include "Scaling.h"
#include "Translation.h"
#include "Triangle.h"
#include "Vec3.h"
#include "Vec4.h"
#include "Matrix4.h"

using namespace std;

class Scene
{
public:
	Color backgroundColor;
	bool cullingEnabled;
	int projectionType;

	vector< vector<Color> > image;
	vector< Camera* > cameras;
	vector< Vec3* > vertices;
	vector< Color* > colorsOfVertices;
	vector< Scaling* > scalings;
	vector< Rotation* > rotations;
	vector< Translation* > translations;
	vector< Model* > models;

	Matrix4 M_cam;
	Matrix4 M_orth_or_per;
	Matrix4 M_vp;

	Scene(const char *xmlPath);

	void initializeImage(Camera* camera);
	void forwardRenderingPipeline(Camera* camera);
	int makeBetweenZeroAnd255(double value);
	void writeImageToPPMFile(Camera* camera);
	void convertPPMToPNG(string ppmFileName, int osType);

	void constructConstantMatrices(Camera *camera);
	void rasterization(Vec4 p0, Vec4 p1, int horRes, int verRes);
	void fillInside(Vec4 v0, Vec4 v1, Vec4 v2, int horRes, int verRes);
	double f(double x, double y, Vec4 p0, Vec4 p1);
	bool isCulled(Vec4 v0, Vec4 v1, Vec4 v2, Vec3 pos);
	bool isInScene(Vec4& p0, Vec4& p1);
	bool isVisible(double den, double num, double& tE, double& tL);
	void drawLine(Vec4 v0, Vec4 v1, Camera* camera);
	void perDiv(Vec4 v);
};

#endif
