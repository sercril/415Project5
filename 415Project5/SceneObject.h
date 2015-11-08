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

	

	void Draw();

	void SetTranslation(gmtl::Matrix44f t);
	void SetRotation(gmtl::Quatf r);
	void SetTexture(Texture t);

	gmtl::Matrix44f scale, translation;
	gmtl::Quatf rotation;
	float length, width, depth, radius, specCoefficient, shine;
	VertexArrayObject VAO;

	Texture texture;

private:
	void Init();
};

#endif __SCENE_OBJECT_H__