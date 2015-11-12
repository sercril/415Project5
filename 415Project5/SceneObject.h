#ifndef __SCENE_OBJECT_H__
#define __SCENE_OBJECT_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <array>
#include <string>


#include <GL/glew.h>
#include <GL/freeglut.h>

#include <gmtl\gmtl.h>
#include <gmtl\Matrix.h>

#include "VertexArrayObject.h"

using namespace std;

enum ObjectType
{
	FLOOR = 0,
	BALL,
	LEFT_WALL,
	RIGHT_WALL,
	FRONT_WALL,
	BACK_WALL

};

struct Texture
{
	unsigned int textureWidth, textureHeight;
	unsigned char *imageData;

	Texture() {}
	Texture(unsigned int textureWidth, unsigned int textureHeight, unsigned char *imageData)
	{
		this->textureWidth = textureWidth;
		this->textureHeight = textureHeight;
		this->imageData = imageData;
	}
};

class SceneObject
{

public:

	SceneObject();
	SceneObject(string objectFile, float length, float width, float depth, GLuint program);
	SceneObject(string objectFile,
				float radius,
				GLuint program);
	~SceneObject();

	void Draw(gmtl::Matrix44f viewMatrix, gmtl::Matrix44f projection);

	void AddTranslation(gmtl::Matrix44f t);
	void AddTranslation(gmtl::Vec3f t);
	void AddRotation(gmtl::Quatf r);
	void SetTexture(Texture t);

	gmtl::Vec3f GetPosition();

	float length, width, depth, radius, specCoefficient, shine;
	gmtl::Matrix44f scale, translation, transform;
	gmtl::Quatf rotation;	
	VertexArrayObject VAO;
	SceneObject* parent;
	ObjectType type;
	std::vector<SceneObject *> children;
	Texture texture;

private:
	void Init();

	GLuint specCoefficient_loc, shine_loc, upVector_loc, modelview_loc;
};

#endif __SCENE_OBJECT_H__