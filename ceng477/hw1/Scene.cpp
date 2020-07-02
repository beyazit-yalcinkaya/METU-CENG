#include "Scene.h"
#include "Camera.h"
#include "Light.h"
#include "Material.h"
#include "Shape.h"
#include "tinyxml2.h"

#include <cmath>
#include <thread>
#include <vector>

using namespace tinyxml2;

Vector3f Scene::getColor(Ray ray, ReturnVal closestObject, int recursionDepth) {
	//Ambient
	Vector3f temp = backgroundColor;
	if (closestObject.intersecting) {
		temp = materials[closestObject.matIndex - 1] -> ambientRef * ambientLight;
		Vector3f w_0 = (ray.origin - closestObject.point).normalize();
		for (PointLight* light: lights) {
			Vector3f w_i = ((light -> position) - closestObject.point).normalize();
			Ray shadowRay(closestObject.point + (w_i * shadowRayEps), w_i);
			bool inShadow = false;
			for (Shape* shape: objects) {
				ReturnVal returnVal = shape -> intersect(shadowRay);
				if (returnVal.intersecting && returnVal.t < shadowRay.gett(light -> position)) {
					inShadow = true;
					break;
				}
			}
			if (inShadow)
				continue;
			//Diffuse
			temp = temp + (materials[closestObject.matIndex - 1] -> diffuseRef *
							max((float)0, w_i.dot(closestObject.normalVector))) *
								(light -> computeLightContribution(closestObject.point));
			//Specular
			temp = temp + (materials[closestObject.matIndex - 1] -> specularRef *
					pow(max((float)0, closestObject.normalVector.dot((w_i + w_0).normalize())),
						materials[closestObject.matIndex - 1] -> phongExp)) *
							(light -> computeLightContribution(closestObject.point));
		}
		//Mirror
		Vector3f mirrorRef = materials[closestObject.matIndex - 1] -> mirrorRef;
		if (recursionDepth > 0 && (mirrorRef.x != 0 || mirrorRef.y != 0 || mirrorRef.z != 0)) {
			Vector3f n = closestObject.normalVector;
			Vector3f w_r = (w_0 * (-1) + n * 2 * max((float)0, n.dot(w_0))).normalize();
			Ray newRay(closestObject.point + w_r * shadowRayEps, w_r);
			ReturnVal newClosestObject;
			for (Shape* shape: objects) {
				ReturnVal returnVal = shape -> intersect(newRay);
				if (returnVal.intersecting && returnVal.t < newClosestObject.t) {
					newClosestObject = returnVal;
				}
			}
			if (newClosestObject.intersecting) {
				temp = temp + mirrorRef * getColor(newRay, newClosestObject, recursionDepth - 1);
			}
		}
	}
	return temp;
}

void Scene::setRow(int y, Camera* camera, Image* currentImage) {
	for (int x = 0; x < camera -> imgPlane.nx; x++) {
		ReturnVal closestObject;
		Color pixelColor = Color(backgroundColor.r, backgroundColor.g, backgroundColor.b);
		Ray ray = camera -> getPrimaryRay(x, y);
		for (Shape* shape: objects) {
			ReturnVal returnVal = shape -> intersect(ray);
			if (returnVal.intersecting && returnVal.t < closestObject.t) {
				closestObject = returnVal;
			}
		}
		if (closestObject.intersecting) {
			Vector3f temp = getColor(ray, closestObject, maxRecursionDepth);
			if (temp.x > 255)
				pixelColor.channel[0] = 255;
			else
				pixelColor.channel[0] = (unsigned char)round(temp.x);
				
			if (temp.y > 255)
				pixelColor.channel[1] = 255;
			else
				pixelColor.channel[1] = (unsigned char)round(temp.y);
			if (temp.z > 255)
				pixelColor.channel[2] = 255;
			else
				pixelColor.channel[2] = (unsigned char)round(temp.z);
		}
		currentImage -> setPixelValue(x, y, pixelColor);
	}
	return;
}

/* 
 * Must render the scene from each camera's viewpoint and create an image.
 * You can use the methods of the Image class to save the image as a PPM file. 
 */
void Scene::renderScene(void) {
	Image* currentImage;
	for (Camera* camera: cameras) {
		currentImage = new Image(camera -> imgPlane.nx, camera -> imgPlane.ny);
		int y = 0;
		while (y < camera -> imgPlane.ny) {
			int threadLimit = std::thread::hardware_concurrency();
			std::vector<std::thread> threads;
			for (int i = 0; i < threadLimit; i++)
				if (y + i < camera -> imgPlane.ny)
					threads.push_back(std::thread(&Scene::setRow, this, y + i, camera, currentImage));
			for (int i = 0; i < threadLimit; i++)
				if (y + i < camera -> imgPlane.ny)
					threads[i].join();
			y += threadLimit;
		}
		currentImage -> saveImage(camera -> imageName);
		delete currentImage;
	}
}

// Parses XML file. 
Scene::Scene(const char *xmlPath)
{
	const char *str;
	XMLDocument xmlDoc;
	XMLError eResult;
	XMLElement *pElement;

	maxRecursionDepth = 1;
	shadowRayEps = 0.001;

	eResult = xmlDoc.LoadFile(xmlPath);

	XMLNode *pRoot = xmlDoc.FirstChild();

	pElement = pRoot->FirstChildElement("MaxRecursionDepth");
	if(pElement != nullptr)
		pElement->QueryIntText(&maxRecursionDepth);

	pElement = pRoot->FirstChildElement("BackgroundColor");
	str = pElement->GetText();
	sscanf(str, "%f %f %f", &backgroundColor.r, &backgroundColor.g, &backgroundColor.b);

	pElement = pRoot->FirstChildElement("ShadowRayEpsilon");
	if(pElement != nullptr)
		pElement->QueryFloatText(&shadowRayEps);

	pElement = pRoot->FirstChildElement("IntersectionTestEpsilon");
	if(pElement != nullptr)
		eResult = pElement->QueryFloatText(&intTestEps);

	// Parse cameras
	pElement = pRoot->FirstChildElement("Cameras");
	XMLElement *pCamera = pElement->FirstChildElement("Camera");
	XMLElement *camElement;
	while(pCamera != nullptr)
	{
        int id;
        char imageName[64];
        Vector3f pos, gaze, up;
        ImagePlane imgPlane;

		eResult = pCamera->QueryIntAttribute("id", &id);
		camElement = pCamera->FirstChildElement("Position");
		str = camElement->GetText();
		sscanf(str, "%f %f %f", &pos.x, &pos.y, &pos.z);
		camElement = pCamera->FirstChildElement("Gaze");
		str = camElement->GetText();
		sscanf(str, "%f %f %f", &gaze.x, &gaze.y, &gaze.z);
		camElement = pCamera->FirstChildElement("Up");
		str = camElement->GetText();
		sscanf(str, "%f %f %f", &up.x, &up.y, &up.z);
		camElement = pCamera->FirstChildElement("NearPlane");
		str = camElement->GetText();
		sscanf(str, "%f %f %f %f", &imgPlane.left, &imgPlane.right, &imgPlane.bottom, &imgPlane.top);
		camElement = pCamera->FirstChildElement("NearDistance");
		eResult = camElement->QueryFloatText(&imgPlane.distance);
		camElement = pCamera->FirstChildElement("ImageResolution");	
		str = camElement->GetText();
		sscanf(str, "%d %d", &imgPlane.nx, &imgPlane.ny);
		camElement = pCamera->FirstChildElement("ImageName");
		str = camElement->GetText();
		strcpy(imageName, str);

		cameras.push_back(new Camera(id, imageName, pos, gaze, up, imgPlane));

		pCamera = pCamera->NextSiblingElement("Camera");
	}

	// Parse materals
	pElement = pRoot->FirstChildElement("Materials");
	XMLElement *pMaterial = pElement->FirstChildElement("Material");
	XMLElement *materialElement;
	while(pMaterial != nullptr)
	{
		materials.push_back(new Material());

		int curr = materials.size() - 1;
	
		eResult = pMaterial->QueryIntAttribute("id", &materials[curr]->id);
		materialElement = pMaterial->FirstChildElement("AmbientReflectance");
		str = materialElement->GetText();
		sscanf(str, "%f %f %f", &materials[curr]->ambientRef.r, &materials[curr]->ambientRef.g, &materials[curr]->ambientRef.b);
		materialElement = pMaterial->FirstChildElement("DiffuseReflectance");
		str = materialElement->GetText();
		sscanf(str, "%f %f %f", &materials[curr]->diffuseRef.r, &materials[curr]->diffuseRef.g, &materials[curr]->diffuseRef.b);
		materialElement = pMaterial->FirstChildElement("SpecularReflectance");
		str = materialElement->GetText();
		sscanf(str, "%f %f %f", &materials[curr]->specularRef.r, &materials[curr]->specularRef.g, &materials[curr]->specularRef.b);
		materialElement = pMaterial->FirstChildElement("MirrorReflectance");
		if(materialElement != nullptr)
		{
			str = materialElement->GetText();
			sscanf(str, "%f %f %f", &materials[curr]->mirrorRef.r, &materials[curr]->mirrorRef.g, &materials[curr]->mirrorRef.b);
		}
				else
		{
			materials[curr]->mirrorRef.r = 0.0;
			materials[curr]->mirrorRef.g = 0.0;
			materials[curr]->mirrorRef.b = 0.0;
		}
		materialElement = pMaterial->FirstChildElement("PhongExponent");
		if(materialElement != nullptr)
			materialElement->QueryIntText(&materials[curr]->phongExp);

		pMaterial = pMaterial->NextSiblingElement("Material");
	}

	// Parse vertex data
	pElement = pRoot->FirstChildElement("VertexData");
	int cursor = 0;
	Vector3f tmpPoint;
	str = pElement->GetText();
	while(str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
		cursor++;
	while(str[cursor] != '\0')
	{
		for(int cnt = 0 ; cnt < 3 ; cnt++)
		{
			if(cnt == 0)
				tmpPoint.x = atof(str + cursor);
			else if(cnt == 1)
				tmpPoint.y = atof(str + cursor);
			else
				tmpPoint.z = atof(str + cursor);
			while(str[cursor] != ' ' && str[cursor] != '\t' && str[cursor] != '\n')
				cursor++; 
			while(str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
				cursor++;
		}
		vertices.push_back(tmpPoint);
	}

	// Parse objects
	pElement = pRoot->FirstChildElement("Objects");
	
	// Parse spheres
	XMLElement *pObject = pElement->FirstChildElement("Sphere");
	XMLElement *objElement;
	while(pObject != nullptr)
	{
		int id;
		int matIndex;
		int cIndex;
		float R;

		eResult = pObject->QueryIntAttribute("id", &id);
		objElement = pObject->FirstChildElement("Material");
		eResult = objElement->QueryIntText(&matIndex);
		objElement = pObject->FirstChildElement("Center");
		eResult = objElement->QueryIntText(&cIndex);
		objElement = pObject->FirstChildElement("Radius");
		eResult = objElement->QueryFloatText(&R);

		objects.push_back(new Sphere(id, matIndex, cIndex, R));

		pObject = pObject->NextSiblingElement("Sphere");
	}

	// Parse triangles
	pObject = pElement->FirstChildElement("Triangle");
	while(pObject != nullptr)
	{
		int id;
		int matIndex;
		int p1Index;
		int p2Index;
		int p3Index;

		eResult = pObject->QueryIntAttribute("id", &id);
		objElement = pObject->FirstChildElement("Material");
		eResult = objElement->QueryIntText(&matIndex);
		objElement = pObject->FirstChildElement("Indices");
		str = objElement->GetText();
		sscanf(str, "%d %d %d", &p1Index, &p2Index, &p3Index);

		objects.push_back(new Triangle(id, matIndex, p1Index, p2Index, p3Index));

		pObject = pObject->NextSiblingElement("Triangle");
	}

	// Parse meshes
	pObject = pElement->FirstChildElement("Mesh");
	while(pObject != nullptr)
	{
		int id;
		int matIndex;
		int p1Index;
		int p2Index;
		int p3Index;
		int cursor = 0;
		int vertexOffset = 0;
		vector<Triangle> faces;

		eResult = pObject->QueryIntAttribute("id", &id);
		objElement = pObject->FirstChildElement("Material");
		eResult = objElement->QueryIntText(&matIndex);
		objElement = pObject->FirstChildElement("Faces");
		objElement->QueryIntAttribute("vertexOffset", &vertexOffset);
		str = objElement->GetText();
		while(str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
			cursor++;
		while(str[cursor] != '\0')
		{
			for(int cnt = 0 ; cnt < 3 ; cnt++)
			{
				if(cnt == 0)
					p1Index = atoi(str + cursor) + vertexOffset;
				else if(cnt == 1)
					p2Index = atoi(str + cursor) + vertexOffset;
				else
					p3Index = atoi(str + cursor) + vertexOffset;
				while(str[cursor] != ' ' && str[cursor] != '\t' && str[cursor] != '\n')
					cursor++; 
				while(str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
					cursor++;
			}
			faces.push_back(*(new Triangle(-1, matIndex, p1Index, p2Index, p3Index)));
		}

		objects.push_back(new Mesh(id, matIndex, faces));

		pObject = pObject->NextSiblingElement("Mesh");
	}

	// Parse lights
	int id;
	Vector3f position;
	Vector3f intensity;
	pElement = pRoot->FirstChildElement("Lights");

	XMLElement *pLight = pElement->FirstChildElement("AmbientLight");
	XMLElement *lightElement;
	str = pLight->GetText();
	sscanf(str, "%f %f %f", &ambientLight.r, &ambientLight.g, &ambientLight.b);

	pLight = pElement->FirstChildElement("PointLight");
	while(pLight != nullptr)
	{
		eResult = pLight->QueryIntAttribute("id", &id);
		lightElement = pLight->FirstChildElement("Position");
		str = lightElement->GetText();
		sscanf(str, "%f %f %f", &position.x, &position.y, &position.z);
		lightElement = pLight->FirstChildElement("Intensity");
		str = lightElement->GetText();
		sscanf(str, "%f %f %f", &intensity.r, &intensity.g, &intensity.b);

		lights.push_back(new PointLight(position, intensity));

		pLight = pLight->NextSiblingElement("PointLight");
	}
}

