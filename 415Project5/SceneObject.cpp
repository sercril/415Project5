#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <gmtl\gmtl.h>
#include <gmtl\Matrix.h>

#include "SceneObject.h"

using namespace std;

SceneObject::SceneObject()
{

}

SceneObject::SceneObject(float length, float width, float depth, GLuint program)
{
	this->length = length;
	this->width = width;
	this->depth = depth;

	this->scale = gmtl::makeScale<gmtl::Matrix44f>(gmtl::Vec3f(this->length, this->width, this->depth));
	this->scale.setState(gmtl::Matrix44f::AFFINE);

	#pragma region Data Vectors

	std::vector<GLfloat> vbuffer_data = 
	{
		1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f
	};

	std::vector<GLfloat> normal_data =
	{
		-0.0f, -1.0f, -0.0f,
		-0.0f, -1.0f, -0.0f,
		-0.0f, -1.0f, -0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, -0.0f,
		1.0f, 0.0f, -0.0f,
		1.0f, 0.0f, -0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, -0.0f, -1.0f,
		0.0f, -0.0f, -1.0f,
		0.0f, -0.0f, -1.0f,
		0.0f, 1.0f, -0.0f,
		0.0f, 1.0f, -0.0f,
		0.0f, 1.0f, -0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		-1.0f, -0.0f, -0.0f,
		-1.0f, -0.0f, -0.0f,
		-1.0f, -0.0f, -0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f
	};

	std::vector<GLfloat> uv_data = 
	{ 
		1.0f, 0.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};

	std::vector<GLushort> index_buffer_data =
	{
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
		9, 10, 11,
		12, 13, 14,
		15, 16, 17,
		18, 19, 20,
		21, 22, 23,
		24, 25, 26,
		27, 28, 29,
		30, 31, 32,
		33, 34, 35,
	};


	#pragma endregion

	this->VAO = VertexArrayObject(vbuffer_data, normal_data, uv_data, index_buffer_data, program);

}

SceneObject::SceneObject(float radius, std::vector<GLfloat> vertex_data, std::vector<GLfloat> normal_data, std::vector<GLfloat> uv_data, std::vector<GLushort> index_data, GLuint program)
{

	this->radius = radius;	

	gmtl::identity(this->translation);
	this->rotation = gmtl::Quatf(0.0f, 0.0f, 0.0f, 1.0f);

	this->scale = gmtl::makeScale<gmtl::Matrix44f>(gmtl::Vec3f(this->radius, this->radius, this->radius));
	this->scale.setState(gmtl::Matrix44f::AFFINE);

	this->VAO = VertexArrayObject(vertex_data, normal_data, uv_data, index_data, program);

}


SceneObject::~SceneObject()
{

}

void SceneObject::Init()
{
	gmtl::identity(this->translation);
	this->rotation = gmtl::Quatf(0.0f, 0.0f, 0.0f, 1.0f);
}

void SceneObject::Draw()
{

}

void SceneObject::SetTexture(Texture t)
{
	this->texture = t;
}

void SceneObject::SetTranslation(gmtl::Matrix44f t)
{
	this->translation = t;
}

void SceneObject::SetRotation(gmtl::Quatf r)
{
	this->rotation = r;
}