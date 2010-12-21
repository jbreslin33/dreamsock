// Simple 3D openGL Library
// Version 2.0

#include <windows.h>
#include <stdio.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>
#include <vector>
#include <math.h>

using namespace std;

float frameInterval = 0.0f;

#include "C3Dcamera.h"
#include "C3Dlib.h"

C3Dcamera::C3Dcamera()
{
	posX	= 0;	posY	= -10;		posZ	= -10;	
	viewX	= 0;	viewY	= -10;		viewZ	= -10;
	upX		= 0;	upY		= 1;		upZ		= 0;
}

void C3Dcamera::MoveTo(float n_posX, float n_posY, float n_posZ,
						float n_viewX, float n_viewY, float n_viewZ,
						float n_upX, float n_upY, float n_upZ)
{
	posX	= n_posX;	posY	= n_posY;	posZ	= n_posZ;	
	viewX	= n_viewX;	viewY	= n_viewY;	viewZ	= n_viewZ;
	upX	= n_upX;		upY	= n_upY;		upZ	= n_upZ;

}

void C3Dcamera::Move(float speed)
{
	speed *= frameInterval;


	float dirX = viewX - posX;
	float dirY = viewY - posY;
	float dirZ = viewZ - posZ;
	
	posX += dirX * speed;	
	posZ += dirZ * speed;	
	viewX += dirX * speed;	
	viewZ += dirZ * speed;
}

void C3Dcamera::Rotate(float angle, float x, float y, float z)
{
	float dirX = viewX - posX;
	float dirY = viewY - posY;
	float dirZ = viewZ - posZ;	

	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);

	// x
	double newViewX  = (cosTheta + (1 - cosTheta) * x * x)		* dirX;
	newViewX += ((1 - cosTheta) * x * y - z * sinTheta)	* dirY;
	newViewX += ((1 - cosTheta) * x * z + y * sinTheta)	* dirZ;

	// y
	double newViewY  = ((1 - cosTheta) * x * y + z * sinTheta)	* dirX;
	newViewY += (cosTheta + (1 - cosTheta) * y * y)		* dirY;
	newViewY += ((1 - cosTheta) * y * z - x * sinTheta)	* dirZ;

	// z
	double newViewZ  = ((1 - cosTheta) * x * z - y * sinTheta)	* dirX;
	newViewZ += ((1 - cosTheta) * y * z + x * sinTheta)	* dirY;
	newViewZ += (cosTheta + (1 - cosTheta) * z * z)		* dirZ;

	viewX = posX + newViewX;
	viewY = posY + newViewY;
	viewZ = posZ + newViewZ;
}

void C3Dcamera::RotateAroundPoint(double centerX, double centerY, double centerZ, float angle, float x, float y, float z)
{
	double distX = posX - centerX;
	double distY = posY - centerY;
	double distZ = posZ - centerZ;

	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);

	double newPosX  = (cosTheta + (1 - cosTheta) * x * x)		* distX;
	newPosX += ((1 - cosTheta) * x * y - z * sinTheta)	* distY;
	newPosX += ((1 - cosTheta) * x * z + y * sinTheta)	* distZ;

	double newPosY  = ((1 - cosTheta) * x * y + z * sinTheta)	* distX;
	newPosY += (cosTheta + (1 - cosTheta) * y * y)		* distY;
	newPosY += ((1 - cosTheta) * y * z - x * sinTheta)	* distZ;

	double newPosZ  = ((1 - cosTheta) * x * z - y * sinTheta)	* distX;
	newPosZ += ((1 - cosTheta) * y * z + x * sinTheta)	* distY;
	newPosZ += (cosTheta + (1 - cosTheta) * z * z)		* distZ;

	posX = centerX + newPosX;
	posY = centerY + newPosY;
	posZ = centerZ + newPosZ;
}

void C3Dcamera::Update()
{
	static float frameTime = 0.0f;				// This stores the last frame's time

    float currentTime = timeGetTime() * 0.001f;				

 	frameInterval = currentTime - frameTime;
	frameTime = currentTime;

	gluLookAt(posX, posY, posZ, viewX, viewY, viewZ, upX, upY, upZ);
}