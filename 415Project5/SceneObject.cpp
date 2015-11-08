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

SceneObject::SceneObject(string objectFile, float length, float width, float depth, GLuint program)
{
	this->length = length;
	this->width = width;
	this->depth = depth;

	this->scale = gmtl::makeScale<gmtl::Matrix44f>(gmtl::Vec3f(this->length, this->width, this->depth));
	this->scale.setState(gmtl::Matrix44f::AFFINE);

	this->VAO = VertexArrayObject(objectFile, program);

}

SceneObject::SceneObject(string objectFile, float radius, GLuint program)
{

	this->radius = radius;	

	gmtl::identity(this->translation);
	this->rotation = gmtl::Quatf(0.0f, 0.0f, 0.0f, 1.0f);

	this->scale = gmtl::makeScale<gmtl::Matrix44f>(gmtl::Vec3f(this->radius, this->radius, this->radius));
	this->scale.setState(gmtl::Matrix44f::AFFINE);

	this->VAO = VertexArrayObject(objectFile, program);

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