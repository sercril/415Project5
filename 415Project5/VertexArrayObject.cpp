#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <map>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <gmtl\gmtl.h>
#include <gmtl\Matrix.h>


#include "VertexArrayObject.h"

using namespace std;


struct VectorLessThan : binary_function<gmtl::Vec3f,gmtl::Vec3f,bool>
{
	bool operator()(const gmtl::Vec3f& first, const gmtl::Vec3f& second)
	{
		return tie(first[0], first[1], first[2]) < tie(second[0], second[1], second[2]);
	}
};

VertexArrayObject::VertexArrayObject() {}
VertexArrayObject::VertexArrayObject(string objectFile, GLuint program)
{

	BlendObj thisObj = BlendObj(objectFile);

	this->verticies = thisObj.importedVerticies;
	this->index_data = thisObj.importedIndexData;

	this->program = program;

	this->GetData();

	this->matrix_loc = glGetUniformLocation(this->program, "Matrix");
	this->vertposition_loc = glGetAttribLocation(this->program, "vertexPosition");
	this->vertex_UV = glGetAttribLocation(this->program, "vertexUV");
	this->normal_loc = glGetAttribLocation(this->program, "vertexNormal");

	/*** VERTEX ARRAY OBJECT SETUP***/
	// Create/Generate the Vertex Array Object
	glGenVertexArrays(1, &this->vertexArray);
	// Bind the Vertex Array Object
	glBindVertexArray(this->vertexArray);

	// Create/Generate the Vertex Buffer Object for the vertices.
	glGenBuffers(1, &this->vertexBuffer);
	// Bind the Vertex Buffer Object.
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(&this->vertex_data[0])*this->vertex_data.size(), &this->vertex_data[0], GL_DYNAMIC_DRAW);
	// Specify data location and organization
	glVertexAttribPointer(this->vertposition_loc, // This number must match the layout in the shader
		3, // Size
		GL_FLOAT, // Type
		GL_FALSE, // Is normalized
		0, ((void*)0));
	glEnableVertexAttribArray(this->vertposition_loc);
	
	glGenBuffers(1, &this->uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(&this->uv_data[0])*this->uv_data.size(), &this->uv_data[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(this->vertex_UV, 2, GL_FLOAT, GL_FALSE, 0, ((void*)0));
	glEnableVertexAttribArray(this->vertex_UV);

	glGenBuffers(1, &this->normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(&this->normal_data[0])*this->normal_data.size(), &this->normal_data[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(this->normal_loc, 3, GL_FLOAT, GL_FALSE, 0, ((void*)0));
	glEnableVertexAttribArray(this->normal_loc);
	
	glGenBuffers(1, &this->indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(&this->index_data[0])*this->index_data.size(),
		&this->index_data[0], GL_DYNAMIC_DRAW);

}

VertexArrayObject::~VertexArrayObject()
{
}


void VertexArrayObject::GetData()
{
	this->vertex_data.clear();
	this->normal_data.clear();
	this->uv_data.clear();
	for (std::vector<Vertex>::iterator it = this->verticies.begin(); it < this->verticies.end(); ++it)
	{
		this->vertex_data.push_back(it->position[0]);
		this->vertex_data.push_back(it->position[1]);
		this->vertex_data.push_back(it->position[2]);

		this->normal_data.push_back(it->normal[0]);
		this->normal_data.push_back(it->normal[1]);
		this->normal_data.push_back(it->normal[2]);

		this->uv_data.push_back(it->u);
		this->uv_data.push_back(it->v);
	}

}

void VertexArrayObject::GenerateSmoothNormals()
{
	
	std::vector<GLfloat> newNormals;
	std::map<gmtl::Vec3f, gmtl::Vec3f, VectorLessThan> pos2norm;
	
	for (std::vector<Vertex>::iterator it = this->verticies.begin(); it < verticies.end(); ++it)
	{
		pos2norm[(*it).position] = gmtl::Vec3f(0.0f, 0.0f, 0.0f);
	}
	
	for (std::vector<GLushort>::iterator it = this->index_data.begin(); it < this->index_data.end(); it += 3)
	{
		Vertex v0, v1, v2;
		gmtl::Vec3f newNormal, vec1, vec2;


		v0 = this->verticies[*it];
		v1 = this->verticies[*(it+1)];
		v2 = this->verticies[*(it+2)];

		vec1 = v1.position - v0.position;
		vec2 = v2.position - v0.position;
	
		newNormal = gmtl::makeCross(vec1,vec2);

		pos2norm[v0.position] += newNormal;
		pos2norm[v1.position] += newNormal;
		pos2norm[v2.position] += newNormal;

	}

	for (std::vector<Vertex>::iterator it = this->verticies.begin(); it < verticies.end(); ++it)
	{
		(*it).normal = gmtl::makeNormal(pos2norm[(*it).position]);		

		cout << (*it).normal << endl;

		newNormals.push_back((*it).normal[0]);
		newNormals.push_back((*it).normal[1]);
		newNormals.push_back((*it).normal[2]);
	}
	
	glGenBuffers(1, &this->normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(&newNormals[0])*newNormals.size(), &newNormals[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(this->normal_loc, 3, GL_FLOAT, GL_FALSE, 0, ((void*)0));
	glEnableVertexAttribArray(this->normal_loc);

}

void VertexArrayObject::GenerateSplitNormals()
{

	std::vector<GLfloat> newNormals;


	for (std::vector<Vertex>::iterator it = this->verticies.begin(); it < verticies.end(); ++it)
	{
		(*it).normal = gmtl::Vec3f(0.0f, 0.0f, 0.0f);
	}

	for (std::vector<GLushort>::iterator it = this->index_data.begin(); it < this->index_data.end(); it += 3)
	{
		Vertex v0, v1, v2;
		gmtl::Vec3f newNormal, vec1, vec2;


		v0 = this->verticies[*it];
		v1 = this->verticies[*(it + 1)];
		v2 = this->verticies[*(it + 2)];

		vec1 = v1.position - v0.position;
		vec2 = v2.position - v0.position;

		newNormal = gmtl::makeCross(vec1, vec2);

		v0.normal = newNormal;
		v1.normal = newNormal;
		v2.normal = newNormal;

		this->verticies[*it] = v0;
		this->verticies[*(it + 1)] = v1;
		this->verticies[*(it + 2)] = v2;

	}

	for (std::vector<Vertex>::iterator it = this->verticies.begin(); it < verticies.end(); ++it)
	{
		(*it).normal = gmtl::makeNormal((*it).normal);

		newNormals.push_back((*it).normal[0]);
		newNormals.push_back((*it).normal[1]);
		newNormals.push_back((*it).normal[2]);
	}

	glGenBuffers(1, &this->normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(&newNormals[0])*newNormals.size(), &newNormals[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(this->normal_loc, 3, GL_FLOAT, GL_FALSE, 0, ((void*)0));
	glEnableVertexAttribArray(this->normal_loc);

}