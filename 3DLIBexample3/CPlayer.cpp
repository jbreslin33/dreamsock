#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>
#include <gl/glut.h>
#include <vector>
#include <math.h>

using namespace std;

#include <c3dlib.h>
#include <c3dmodel.h>

#include "CPlayer.h"


CPlayer::CPlayer(C3Dmodel *theModel, C3DIMAGE2D theSkin)
{
	this->model = theModel;
	this->skin = theSkin;
	animFrame = 0.0;
	rotation = 0.0;
	posX = 0;
	posZ = 0;
	lookX = 0;
	lookZ = 1;

	AllocConsole();
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
}

void printline(char *string, ...)
{
	char buf[1024];
	char buf2[1024];
	va_list ap;
	va_start(ap, string);
	vsprintf(buf, string, ap);
	va_end(ap);

	sprintf(buf2, "-> %s\n", buf);

	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(console, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	

	WriteConsole(console, buf2, strlen(buf2), NULL, NULL);
}

CPlayer::~CPlayer()
{

}

inline mathsNormalize(float *v) {
	float d = (sqrt((v[0]*v[0]) + (v[1]*v[1]) + (v[2]*v[2])));
	v[0] = v[0] / d;
	v[1] = v[1] / d;
	v[2] = v[2] / d;
}

#define mathsInnerProduct(v,q) \
	((v)[0] * (q)[0] + \
	(v)[1] * (q)[1] + \
	(v)[2] * (q)[2])	


/* a = b x c */

#define mathsCrossProduct(a,b,c) \
	(a)[0] = (b)[1] * (c)[2] - (c)[1] * (b)[2]; \
	(a)[1] = (b)[2] * (c)[0] - (c)[2] * (b)[0]; \
	(a)[2] = (b)[0] * (c)[1] - (c)[0] * (b)[1];



void CPlayer::rotate(double rot)
{
	rotation+=rot;

	lookX = sin(rotation);
	lookZ = -cos(rotation);

	printline("LookX = %f   LookZ = %f", lookX, lookZ);
}

void CPlayer::move(int direction)
{
	posX = posX + direction*(lookX)*0.5;
	posZ = posZ + direction*(lookZ)*0.5;
}

void CPlayer::render(void)
{
	animFrame += 0.005;
	if(animFrame > 1)
		animFrame -= 1;

	glPushMatrix();

		glTranslatef(posX, 0.0,-50+posZ);

		glRotatef(90, 1, 0, 0);
		glRotatef(180, 0, 1, 0);
		glRotatef(90, 0, 0, 1);

		// work out the 'lookat' rotation

		float lookAt[3],objToCamProj[3],upAux[3];

		glPushMatrix();

	// objToCamProj is the vector in world coordinates from the 
	// local origin to the camera projected in the XZ plane
		objToCamProj[0] = lookX - posX ;
		objToCamProj[1] = 0;
		objToCamProj[2] = lookZ - posZ ;

	// This is the original lookAt vector for the object 
	// in world coordinates
		lookAt[0] = 0;
		lookAt[1] = 0;
		lookAt[2] = 1;


	// normalize both vectors to get the cosine directly afterwards
		mathsNormalize(objToCamProj);

	// easy fix to determine wether the angle is negative or positive
	// for positive angles upAux will be a vector pointing in the 
	// positive y direction, otherwise upAux will point downwards
	// effectively reversing the rotation.

		mathsCrossProduct(upAux,lookAt,objToCamProj);

	// compute the angle
		double angleCosine = mathsInnerProduct(lookAt,objToCamProj);

	// perform the rotation. The if statement is used for stability reasons
	// if the lookAt and objToCamProj vectors are too close together then 
	// |angleCosine| could be bigger than 1 due to lack of precision
	   if ((angleCosine < 0.99990) && (angleCosine > -0.9999))
		  glRotatef(acos(angleCosine)*180/3.14, 0, 0, 1);	

//		printline("ang = %f", ang);

	//	glRotatef(ang, 0, 0, 1);

		glBindTexture(GL_TEXTURE_2D, skin);

		if(model != NULL)
			model->render(animFrame);

	glPopMatrix();
}