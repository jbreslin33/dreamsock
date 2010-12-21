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
#include <c3dmaths.h>

#include "CPlayer.h"


CPlayer::CPlayer(C3Dmodel *theModel, C3DIMAGE2D theSkin)
{
	this->model = theModel;
	this->skin = theSkin;
	animFrame = 0.0;
	posX = 0;
	posY = 0;
	posZ = 0;
	lookX = 0;
	lookY = 0;
	lookZ = 1;
	angleX = 0;
	angleY = 0;
	angleZ = 0;
}

CPlayer::~CPlayer()
{

}

void CPlayer::Rotate(float angle, float x, float y, float z)
{
	float dirX = lookX - posX;
	float dirY = lookY - posY;
	float dirZ = lookZ - posZ;	

	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);

	// x
	double newLookX  = (cosTheta + (1 - cosTheta) * x * x)		* dirX;
	newLookX += ((1 - cosTheta) * x * y - z * sinTheta)	* dirY;
	newLookX += ((1 - cosTheta) * x * z + y * sinTheta)	* dirZ;

	// y
	double newLookY  = ((1 - cosTheta) * x * y + z * sinTheta)	* dirX;
	newLookY += (cosTheta + (1 - cosTheta) * y * y)		* dirY;
	newLookY += ((1 - cosTheta) * y * z - x * sinTheta)	* dirZ;

	// z
	double newLookZ  = ((1 - cosTheta) * x * z - y * sinTheta)	* dirX;
	newLookZ += ((1 - cosTheta) * y * z + x * sinTheta)	* dirY;
	newLookZ += (cosTheta + (1 - cosTheta) * z * z)		* dirZ;

	lookX = posX + newLookX;
	lookY = posY + newLookY;
	lookZ = posZ + newLookZ;
}

void CPlayer::Move(float speed)
{
	float dirX = lookX - posX;
	float dirY = lookY - posY;
	float dirZ = lookZ - posZ;
	
	posX += dirX * speed;	
	posZ += dirZ * speed;	
	lookX += dirX * speed;	
	lookZ += dirZ * speed;
}

void CPlayer::MoveTo(float newPosX, float newPosY, float newPosZ)
{
	float dirX = lookX - posX;
	float dirY = lookY - posY;
	float dirZ = lookZ - posZ;
	
	posX = newPosX;
	posY = newPosY;
	posZ = newPosZ;

	lookX = newPosX + dirX;
	lookY = newPosY + dirY;
	lookZ = newPosZ + dirZ;
}

void CPlayer::Render(void)
{
	animFrame += (1 / (double) (model->animFrameCount[model->currentAnimation]*6.0));
	if(animFrame > 1)
		animFrame -= 1;

	glPushMatrix();
	{


		glTranslatef(posX, posY, posZ);

		glRotatef(90, 1, 0, 0);
		glRotatef(180, 0, 1, 0);
		glRotatef(90, 0, 0, 1);

		float lookAt[3]={0,0,1},objToCamProj[3],upAux[3],angleCosine;
		objToCamProj[0] = lookX - posX;
		objToCamProj[1] = 0;
		objToCamProj[2] = lookZ - posZ;
		mathsNormalize(objToCamProj);
		mathsCrossProduct(upAux,lookAt,objToCamProj);

		angleCosine = mathsInnerProduct(lookAt,objToCamProj);
		glRotatef(acos(angleCosine)*180/3.14, upAux[0], upAux[2], upAux[1]);	


		glBindTexture(GL_TEXTURE_2D, skin);
		glScalef(0.2, 0.2, 0.2);

		

		if(model != NULL)
			model->render(animFrame);
	}
	glPopMatrix();
}