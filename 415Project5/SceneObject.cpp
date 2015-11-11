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

	this->Init();

}

SceneObject::SceneObject(string objectFile, float radius, GLuint program)
{

	this->radius = radius;

	this->scale = gmtl::makeScale<gmtl::Matrix44f>(gmtl::Vec3f(this->radius, this->radius, this->radius));
	this->scale.setState(gmtl::Matrix44f::AFFINE);

	this->VAO = VertexArrayObject(objectFile, program);

	this->Init();

}


SceneObject::~SceneObject()
{

}

void SceneObject::Init()
{
	gmtl::identity(this->translation);
	gmtl::identity(this->transform);
	this->rotation = gmtl::Quatf(0.0f, 0.0f, 0.0f, 1.0f);


	this->upVector_loc = glGetUniformLocation(this->VAO.program, "upVector");
	this->specCoefficient_loc = glGetUniformLocation(this->VAO.program, "specCoefficient");
	this->shine_loc = glGetUniformLocation(this->VAO.program, "shine");	
	this->modelview_loc = glGetUniformLocation(this->VAO.program, "modelview");

	this->specCoefficient = 0.1f;
	this->shine = 0.5f;


}

void SceneObject::Draw(gmtl::Matrix44f viewMatrix, gmtl::Matrix44f projection)
{
	gmtl::Matrix44f rotation = gmtl::makeRot<gmtl::Matrix44f>(gmtl::EulerAngleXYZf(this->rotation[0], this->rotation[1], this->rotation[2]));
	gmtl::Matrix44f newMV = viewMatrix * rotation * this->translation * this->scale;
	gmtl::Matrix44f render = projection * newMV;


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->texture.textureHeight, this->texture.textureWidth, 0, GL_RGB, GL_UNSIGNED_BYTE, this->texture.imageData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	
	glUniformMatrix4fv(this->VAO.matrix_loc, 1, GL_FALSE, &render[0][0]);


	glUniform3f(this->upVector_loc, viewMatrix[1][0], viewMatrix[1][1], viewMatrix[1][2]);
	glUniformMatrix4fv(this->modelview_loc, 1, GL_FALSE, &newMV[0][0]);
	glUniform1f(this->specCoefficient_loc, this->specCoefficient);
	glUniform1f(this->shine_loc, this->shine);

	// Draw the transformed cuboid
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(0xFFFF);
	glDrawElements(GL_TRIANGLES, this->VAO.index_data.size(), GL_UNSIGNED_SHORT, NULL);

	if (!this->children.empty())
	{
		for (std::vector<SceneObject *>::iterator it = this->children.begin();
			it < this->children.end();
			++it)
		{
			(*it)->Draw(newMV, projection);
		}		
	}
}

void SceneObject::SetTexture(Texture t)
{
	this->texture = t;
}

void SceneObject::SetTranslation(gmtl::Matrix44f t)
{
	this->translation = this->translation * t;
}

void SceneObject::SetRotation(gmtl::Quatf r)
{
	this->rotation = this->rotation * r;
}