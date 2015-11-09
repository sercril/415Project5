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

	this->Init();

	this->scale = gmtl::makeScale<gmtl::Matrix44f>(gmtl::Vec3f(this->length, this->width, this->depth));
	this->scale.setState(gmtl::Matrix44f::AFFINE);

	this->VAO = VertexArrayObject(objectFile, program);

}

SceneObject::SceneObject(string objectFile, float radius, GLuint program)
{

	this->radius = radius;

	this->Init();

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

	this->specCoefficient_loc = glGetUniformLocation(this->VAO.program, "specCoefficient");
	this->shine_loc = glGetUniformLocation(this->VAO.program, "shine");
	this->upVector_loc = glGetUniformLocation(this->VAO.program, "upVector");
	this->modelview_loc = glGetUniformLocation(this->VAO.program, "modelview");

	this->specCoefficient = 0.2f;
	this->shine = 0.1f;

	glUniform1f(this->specCoefficient_loc, this->specCoefficient);
	glUniform1f(this->shine_loc, this->shine);

}

void SceneObject::Draw(gmtl::Matrix44f viewMatrix, gmtl::Matrix44f projection)
{
	gmtl::Matrix44f rotation = gmtl::makeTrans<gmtl::Matrix44f>(gmtl::Vec3f(this->rotation[0], this->rotation[1], this->rotation[2]));
	gmtl::Matrix44f newMV = viewMatrix * rotation * this->translation * this->scale;
	gmtl::Matrix44f render = projection * newMV;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->texture.textureHeight, this->texture.textureWidth, 0, GL_RGB, GL_UNSIGNED_BYTE, this->texture.imageData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindVertexArray(this->VAO.vertexArray);
	glUniformMatrix4fv(this->VAO.matrix_loc, 1, GL_FALSE, &render[0][0]);

	glUniform4f(this->upVector_loc, viewMatrix[1][0], viewMatrix[1][1], viewMatrix[1][2], 0);
	glUniformMatrix4fv(modelview_loc, 1, GL_FALSE, &newMV[0][0]);

	// Draw the transformed cuboid
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(0xFFFF);
	glDrawElements(GL_TRIANGLES, this->VAO.index_data.size(), GL_UNSIGNED_SHORT, NULL);

	if (!graph[i]->children.empty())
	{
		renderGraph(graph[i]->children, newMV);
	}
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