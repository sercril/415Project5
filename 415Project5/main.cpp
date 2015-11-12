#define USE_PRIMITIVE_RESTART
#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sstream>
#include <fstream>
#include <stack>
#include <functional>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <gmtl\gmtl.h>
#include <gmtl\Matrix.h>

#include "LoadShaders.h"
#include "SceneObject.h"

#include "Texture.h"

#pragma comment (lib, "glew32.lib")
#pragma warning (disable : 4996) // Windows ; consider instead replacing fopen with fopen_s

using namespace std;

#pragma region Structs and Enums



#pragma endregion

#pragma region "Global Variables"

#define SCREEN_WIDTH 1920.0f
#define SCREEN_HEIGHT 1080.0f
#define NUM_OBJECTS 2
#define INDECIES 10000

int mouseX, mouseY,
mouseDeltaX, mouseDeltaY,
ambientFlag, diffuseFlag, specFlag, texFlag, floorTexFlag, ballTexFlag;

bool outTrans;

float azimuth, elevation, ballRadius, ballDiameter, floorY, cameraZFactor,
		nearValue, farValue, leftValue, rightValue, topValue, bottomValue,
		ballSpec, ballShine, floorSpec, floorShine;


GLuint program, Matrix_loc, vertposition_loc, normal_loc, modelview_loc,
		lightPosition_loc, specCoefficient_loc, upVector_loc, 
		ambientLight_loc, diffuseLight_loc, specularLight_loc, shine_loc,
		ambientFlag_loc, diffuseFlag_loc, specularFlag_loc, texFlag_loc,
		vertex_UV, texture_location, NormalMatrix;

GLenum errCode;

const GLubyte *errString;


gmtl::Matrix44f view, modelView, viewScale, camera, projection, normalMatrix,
				elevationRotation, azimuthRotation, cameraZ, viewRotation;


std::vector<SceneObject*> sceneGraph;
std::vector<Vertex> ballData;

gmtl::Point3f lightPosition, lightPoint;

gmtl::Vec3f ballDelta;



#pragma endregion

#pragma region Camera

float arcToDegrees(float arcLength)
{
	return ((arcLength * 360.0f) / (2.0f * M_PI));
}

float degreesToRadians(float deg)
{
	return (2.0f * M_PI *(deg / 360.0f));
}

void cameraRotate()
{

	elevationRotation.set(
		1, 0, 0, 0,
		0, cos(elevation * -1), (sin(elevation * -1) * -1), 0,
		0, sin(elevation * -1), cos(elevation * -1), 0,
		0, 0, 0, 1);

	azimuthRotation.set(
		cos(azimuth * -1), 0, sin(azimuth * -1), 0,
		0, 1, 0, 0,
		(sin(azimuth * -1) * -1), 0, cos(azimuth * -1), 0,
		0, 0, 0, 1);

	elevationRotation.setState(gmtl::Matrix44f::ORTHOGONAL);

	azimuthRotation.setState(gmtl::Matrix44f::ORTHOGONAL);


	viewRotation = azimuthRotation * elevationRotation;
	gmtl::invert(viewRotation);

	view = azimuthRotation * elevationRotation * cameraZ;

	gmtl::invert(view);

	glutPostRedisplay();
}

#pragma endregion

#pragma region Helper Functions


bool IsWall(SceneObject* obj)
{
	switch (obj->type)
	{
	case FRONT_WALL:
	case BACK_WALL:
	case LEFT_WALL:
	case RIGHT_WALL:
		return true;
	}

	return false;
}

// TODO Move this to SceneObject.
Texture LoadTexture(char* filename)
{
	unsigned int textureWidth, textureHeight;
	unsigned char *imageData;

	LoadPPM(filename, &textureWidth, &textureHeight, &imageData, 1);
	return Texture(textureWidth, textureHeight, imageData);
}

// TODO Generalize this.
SceneObject* AddWall(int i)
{
	SceneObject* wall = new SceneObject();

	switch (i)
	{
		case 0:
			wall = new SceneObject("OBJs/cube.obj", ballDiameter * 10.0f, ballDiameter+2.0f, 2.0f, program);
			wall->AddTranslation(gmtl::Vec3f(0.0f, 0.0f, ((ballDiameter * 10.0f)*2.0f) + 2.0f));
			wall->type = BACK_WALL;
			break;

		case 1:
			wall = new SceneObject("OBJs/cube.obj", ballDiameter * 10.0f, ballDiameter + 2.0f, 2.0f, program);
			wall->AddTranslation(gmtl::Vec3f(0.0f, 0.0f, (-(ballDiameter * 10.0f)*2.0f) - 2.0f));
			wall->type = FRONT_WALL;
			break;

		case 2:
			wall = new SceneObject("OBJs/cube.obj", 2.0f, ballDiameter + 2.0f, (ballDiameter * 10.0f)*2.0f, program);
			wall->AddTranslation(gmtl::Vec3f((ballDiameter * 10.0f)+ 2.0f, 0.0f, 0.0f));
			wall->type = RIGHT_WALL;
			break;

		case 3:
			wall = new SceneObject("OBJs/cube.obj", 2.0f, ballDiameter + 2.0f, (ballDiameter * 10.0f)*2.0f, program);
			wall->AddTranslation(gmtl::Vec3f((-(ballDiameter * 10.0f)) - 2.0f, 0.0f, 0.0f));
			wall->type = LEFT_WALL;
			break;
	}

	wall->parent = NULL;
	wall->children.clear();
	wall->SetTexture(LoadTexture("textures/dirt.ppm"));
	wall->AddTranslation(gmtl::Vec3f(0.0f, 9.0f, 0.0f));



	return wall;
}

void buildGraph()
{
	
	SceneObject* ball = new SceneObject("OBJs/smoothSphere.obj", ballRadius, program);
	SceneObject* floor = new SceneObject("OBJs/cube.obj", ballDiameter * 10.0f, 1.0f, (ballDiameter * 10.0f)*2.0f, program);
	gmtl::Matrix44f initialTranslation;
	gmtl::Quatf initialRotation;

		
	//Ball
	ball->type = BALL;
	ball->parent = NULL; 
	ball->children.clear();

	initialTranslation = gmtl::makeTrans<gmtl::Matrix44f>(gmtl::Vec3f(0.0f, ballRadius, 0.0f));
	initialTranslation.setState(gmtl::Matrix44f::TRANS);
	ball->AddTranslation(initialTranslation);
	ball->SetTexture(LoadTexture("textures/moonmap.ppm"));

	sceneGraph.push_back(ball);

	//Floor
	floor->type = FLOOR;
	floor->parent = NULL;
	floor->children.clear();
	initialTranslation = gmtl::makeTrans<gmtl::Matrix44f>(gmtl::Vec3f(0.0f,-1.0f,0.0f));
	initialTranslation.setState(gmtl::Matrix44f::TRANS);
	//floor->AddTranslation(initialTranslation);
	floor->SetTexture(LoadTexture("textures/carpet.ppm"));

	sceneGraph.push_back(floor);

	for (int i = 0; i < 4; ++i)
	{

		sceneGraph.push_back(AddWall(i));
	}

	
}


bool IsCollided(SceneObject* obj1, SceneObject* obj2)
{
	gmtl::Vec3f posDiff;
	float collisionDiff;
	if (IsWall(obj1) && obj2->type == BALL)
	{
		
		posDiff = obj1->GetPosition() - obj2->GetPosition();

		switch (obj1->type)
		{
			case FRONT_WALL:
			case BACK_WALL:
				collisionDiff = obj1->depth + obj2->radius;
				if (abs(posDiff[2]) < collisionDiff)
				{
					return true;
				}
				break;

			case LEFT_WALL:
			case RIGHT_WALL:
				collisionDiff = obj1->length + obj2->radius;
				if (abs(posDiff[2]) < collisionDiff)
				{
					return true;
				}
				break;
		}
		
	}
	else if (obj1->type == BALL && IsWall(obj2))
	{

	}
	else if (obj1->type == BALL && obj2->type == BALL)
	{

	}

	return false;
}

// TODO Combine Traverse and Render somehow
/*void traverseGraph(std::vector<SceneObject*> graph)
{
	if (!graph.empty())
	{
		for (int i = 0; i < graph.size(); ++i)
		{
			if (!graph[i]->children.empty())
			{
				for (std::vector<SceneObject *>::iterator it = graph[i]->children.begin();
					it < graph[i]->children.end();
					++it)
				{
					traverseGraph((*it)->children);
				}
			}



		}
	}
} */


void HandleCollisions()
{
	SceneObject* checkObj;


	for (std::vector<SceneObject*>::iterator it = sceneGraph.begin(); it < sceneGraph.end(); ++it)
	{
		checkObj = (*it);
		for (std::vector<SceneObject*>::iterator innerIt = sceneGraph.begin(); innerIt < sceneGraph.end(); ++innerIt)
		{
			if (checkObj != (*it))
			{
				if (IsCollided(checkObj, (*it)))
				{
					
				}
			}
		}

	}
}

void renderGraph(std::vector<SceneObject*> graph, gmtl::Matrix44f mv)
{
	int j = 0;
	
	if(!graph.empty())
	{
		for (int i = 0; i < graph.size(); ++i)
		{
			
			//Should all be graph[i]->draw();

			switch (graph[i]->type)
			{
				case BALL:
					graph[i]->AddTranslation(gmtl::makeTrans<gmtl::Matrix44f>(ballDelta));
					if (outTrans)
					{
						cout << graph[i]->GetPosition() << endl;
					}
					break;

				default:

					if (IsWall(graph[i]))
					{
						if (ballDelta != gmtl::Vec3f(0, 0, 0))
						{
							cout << "WALL " << j << ": " << sceneGraph[0]->GetPosition() - graph[i]->GetPosition() << endl;
						}

						++j;
					}
					
					break;
			}
			glBindVertexArray(graph[i]->VAO.vertexArray);
			// Send a different transformation matrix to the shader
			
			glUniformMatrix4fv(NormalMatrix, 1, GL_FALSE, &viewRotation[0][0]);
			

			lightPoint = mv * lightPosition;
			glUniform3f(lightPosition_loc, lightPoint[0], lightPoint[1], lightPoint[2]);
			
			
			
			glUniform3f(ambientLight_loc, 1.0f, 1.0f, 1.0f);
			glUniform3f(diffuseLight_loc, 1.0f, 1.0f, 1.0f);
			glUniform3f(specularLight_loc, 1.0f, 1.0f, 1.0f);

			graph[i]->Draw(mv, projection);

			

		}
	}
	
	return;
}

#pragma endregion

#pragma region "Input"

# pragma region "Mouse Input"

void mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
	{
		mouseX = x;
		mouseY = y;
	}
}

void mouseMotion(int x, int y)
{

	mouseDeltaX = x - mouseX;
	mouseDeltaY = y - mouseY;


	elevation += degreesToRadians(arcToDegrees(mouseDeltaY)) / (SCREEN_HEIGHT/2);
	azimuth += degreesToRadians(arcToDegrees(mouseDeltaX)) / (SCREEN_WIDTH /2 );

	cameraRotate();

	mouseX = x;
	mouseY = y;

}

# pragma endregion

#pragma region "Keyboard Input"

void keyboard(unsigned char key, int x, int y)
{
	switch (key) 
	{

		

		case 'w':
			ballDelta = gmtl::Vec3f(0, 1.0f, 0);
			break;
		case 'a':
			ballDelta = gmtl::Vec3f(-1.0f, 0, 0);
			break;
		case 's':
			ballDelta = gmtl::Vec3f(0, -1.0f, 0);
			break;
		case 'd':
			ballDelta = gmtl::Vec3f(1.0f, 0, 0);
			break;
		case 'q':
			ballDelta = gmtl::Vec3f(0, 0, -1.0f);
			break;
		case 'e':
			ballDelta = gmtl::Vec3f(0, 0, 1.0f);
			break;

		case 'p':
			outTrans = true;
		
			break;


		case 'Z':
			cameraZFactor += 10.f;
			cameraZ = gmtl::makeTrans<gmtl::Matrix44f>(gmtl::Vec3f(0.0f, 0.0f, cameraZFactor));
			cameraZ.setState(gmtl::Matrix44f::TRANS);
			view = azimuthRotation * elevationRotation * cameraZ;
			gmtl::invert(view);			
			break;

		case 'z':
			cameraZFactor -= 10.f;
			cameraZ = gmtl::makeTrans<gmtl::Matrix44f>(gmtl::Vec3f(0.0f, 0.0f, cameraZFactor));
			cameraZ.setState(gmtl::Matrix44f::TRANS);
			view = azimuthRotation * elevationRotation * cameraZ;
			gmtl::invert(view);
			break;

		case 033 /* Escape key */:
			exit(EXIT_SUCCESS);
			break;
	}
	
	glutPostRedisplay();
}

#pragma endregion

#pragma endregion

#pragma region "GLUT Functions"

void display()
{

	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	renderGraph(sceneGraph, view);
	//Ask GL to execute the commands from the buffer
	glutSwapBuffers();	// *** if you are using GLUT_DOUBLE, use glutSwapBuffers() instead 

	//Check for any errors and print the error string.
	//NOTE: The string returned by gluErrorString must not be altered.
	if ((errCode = glGetError()) != GL_NO_ERROR) {
		errString = gluErrorString(errCode);
		cout << "OpengGL Error: " << errString << endl;
	}
}

void idle()
{
	ballDelta = gmtl::Vec3f(0, 0, 0);

	outTrans = false;
}

void init()
{

	elevation = azimuth = 0;
	ballRadius = floorY = 4.0f;
	ballDiameter = ballRadius * 2.0f;
	outTrans = false;

	ballShine = floorShine = 0.1f;
	ballSpec = floorSpec = 0.2f;


	// Enable depth test (visible surface determination)
	glEnable(GL_DEPTH_TEST);

	// OpenGL background color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Load/compile/link shaders and set to use for rendering
	ShaderInfo shaders[] = { { GL_VERTEX_SHADER, "Cube_Vertex_Shader.vert" },
	{ GL_FRAGMENT_SHADER, "Cube_Fragment_Shader.frag" },
	{ GL_NONE, NULL } };

	program = LoadShaders(shaders);
	glUseProgram(program);

	//Get the shader parameter locations for passing data to shaders
	
	
	
	NormalMatrix = glGetUniformLocation(program, "NormalMatrix");
	lightPosition_loc = glGetUniformLocation(program, "lightPosition");
	
	ambientLight_loc = glGetUniformLocation(program, "ambientLight");
	diffuseLight_loc = glGetUniformLocation(program, "diffuseLight");
	specularLight_loc = glGetUniformLocation(program, "specularLight");

	glActiveTexture(GL_TEXTURE0);

	texture_location = glGetUniformLocation(program, "texture_Colors");
	glBindTexture(GL_TEXTURE_2D, texture_location);

	

	gmtl::identity(view);
	gmtl::identity(modelView);
	gmtl::identity(viewRotation);

	lightPosition.set(0.0f, 20.0f, 0.0f);

	nearValue = 1.0f;
	farValue = 1000.0f;
	topValue = SCREEN_HEIGHT / SCREEN_WIDTH;
	bottomValue = topValue * -1.0f;
	rightValue = 1.0f;
	leftValue = -1.0f;
	
	normalMatrix.set(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
		);

	projection.set(
		((2.0f * nearValue) / (rightValue - leftValue)), 0.0f, ((rightValue + leftValue) / (rightValue - leftValue)), 0.0f,
		0.0f, ((2.0f * nearValue) / (topValue - bottomValue)), ((topValue + bottomValue) / (topValue - bottomValue)), 0.0f,
		0.0f, 0.0f, ((-1.0f * (farValue + nearValue)) / (farValue - nearValue)), ((-2.0f*farValue*nearValue)/(farValue-nearValue)),
		0.0f,0.0f,-1.0f,0.0f		
		);

	cameraZFactor = 350.0f;

	cameraZ = gmtl::makeTrans<gmtl::Matrix44f>(gmtl::Vec3f(0.0f,0.0f,cameraZFactor));
	cameraZ.setState(gmtl::Matrix44f::TRANS);
	
	elevation = degreesToRadians(30.0f);
	azimuth = 0.0f;
	
	cameraRotate();

	buildGraph();

	ambientFlag = diffuseFlag = specFlag = texFlag = ballTexFlag = floorTexFlag = 1;
	
}

#pragma endregion

int main(int argc, char** argv)
{
	// For more details about the glut calls, 
	// refer to the OpenGL/freeglut Introduction handout.

	//Initialize the freeglut library
	glutInit(&argc, argv);

	//Specify the display mode
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

	//Set the window size/dimensions
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);

	// Specify OpenGL version and core profile.
	// We use 3.3 in thie class, not supported by very old cards
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutCreateWindow("415/515 CUBOID DEMO");

	glewExperimental = GL_TRUE;

	if (glewInit())
		exit(EXIT_FAILURE);

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	glutIdleFunc(idle);

	//Transfer the control to glut processing loop.
	glutMainLoop();

	return 0;
}