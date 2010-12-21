// Simple 3D openGL Library
// Version 2.0


#include <windows.h>
#include <stdio.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>
#include <vector>

using namespace std;

#include "C3Dmodel.h"
#include "C3Dlib.h"




void C3Dmodel::render()
{
	render(0);	
}

void C3Dmodel::render(int frame)
{
	glBegin(GL_TRIANGLES);
	for(int i=0; i<numTriangles; i++)
	{
		for(int j=0; j<3; j++)
		{
			glTexCoord2f( texCoord[ (texIndex[i][j]) ][0]/(double)skinWidth,
						  texCoord[ (texIndex[i][j]) ][1]/(double)skinHeight);
			glVertex3f(vertex[frame][triIndex[i][j]][0], vertex[frame][triIndex[i][j]][1], vertex[frame][triIndex[i][j]][2]);
		}
	}
	glEnd();
}

bool C3Dmodel::setAnimation(int id)
{
	if(id >= 0 && id < animName.size())
	{
		currentAnimation = id;
		return true;
	}
	return false;
}

void C3Dmodel::render(double time)
{
	
	double thisAnim = time * animFrameCount[currentAnimation];
	int thisFrame = (int) thisAnim;
	double frameOffset = thisAnim - thisFrame; 
	thisFrame += animStartFrame[currentAnimation];
	
	int nextFrame = thisFrame+1;
	if(nextFrame == (animStartFrame[currentAnimation]+animFrameCount[currentAnimation]))
		nextFrame = animStartFrame[currentAnimation];
	
	// render
	glBegin(GL_TRIANGLES);
	for(int i=0; i<numTriangles; i++)
	{
		for(int j=0; j<3; j++)
		{
			glTexCoord2f( texCoord[ (texIndex[i][j]) ][0]/(double)skinWidth,
							 texCoord[ (texIndex[i][j]) ][1]/(double)skinHeight);
			glVertex3f(vertex[thisFrame][triIndex[i][j]][0]+(frameOffset*(vertex[nextFrame][triIndex[i][j]][0]-vertex[thisFrame][triIndex[i][j]][0])),
							vertex[thisFrame][triIndex[i][j]][1]+(frameOffset*(vertex[nextFrame][triIndex[i][j]][1]-vertex[thisFrame][triIndex[i][j]][1])),
							vertex[thisFrame][triIndex[i][j]][2]+(frameOffset*(vertex[nextFrame][triIndex[i][j]][2]-vertex[thisFrame][triIndex[i][j]][2])));
		}
	}
	glEnd();
}



void substr(char* to_str, char* from_str, int n1, int n2)
{ 
	int j;
	for(j=0,n1--;(j!=(n2-n1)) && (*from_str!='\0');j++) 
    		to_str[j] = from_str[j+n1];
    	to_str[j]='\0';
}


/*static*/ C3Dmodel *C3Dmodel::loadMD2(char *filename)
{
	// create temporary model...

//	AllocConsole();
//	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	C3Dmodel *model = new C3Dmodel();
	
	// MD2 Header values...
	int magic, version;
	int frameSize;
	int offsetSkins, offsetTexCoords, offsetTriangles, offsetFrames, offsetGlCommands;
	
	
	// check the filename is valid...
	if(filename == NULL || strlen(filename) == 0)
	{
		MessageBox(NULL, "C3Dmodel: Filename was not valid", "Error", MB_OK);
		return NULL;
	}
	
	// attempt to read and store the MD2 header...
	FILE *fp;
	fp = fopen(filename, "rb");
				
		// 1) Magic Number
			fread(&magic, sizeof(int), 1, fp);
//			println("Magic: %i", magic);
				
		// 2) Version
			fread(&version, sizeof(int), 1, fp);
//			println("Version: %i", version);
			
		// 3) Skin Width & Height
			fread(&model->skinWidth, sizeof(int), 1, fp);
			fread(&model->skinHeight, sizeof(int), 1, fp);
//			println("skinWidth: %i", model->skinWidth);
//			println("skinHeight: %i", model->skinHeight);
				
		// 4) Frame Size (bytes)
			fread(&frameSize, sizeof(int), 1, fp);
//			println("frameSize: %i", frameSize);
				
		// 5) Number of X
			fread(&model->numSkins, sizeof(int), 1, fp);
			fread(&model->numVertices, sizeof(int), 1, fp);
			fread(&model->numTexCoords, sizeof(int), 1, fp);
			fread(&model->numTriangles, sizeof(int), 1, fp);
			fread(&model->numGlCommands, sizeof(int), 1, fp);
			fread(&model->numFrames, sizeof(int), 1, fp);
/*			println("numSkins: %i", model->numSkins);
			println("numVertices: %i", model->numVertices);
			println("numTexCoords: %i", model->numTexCoords);
			println("numTriangles: %i", model->numTriangles);
			println("numGlCommands: %i", model->numGlCommands);
			println("numFrames: %i", model->numFrames);*/

			
		// 6) Offsets...
			fread(&offsetSkins, sizeof(int), 1, fp);
			fread(&offsetTexCoords, sizeof(int), 1, fp);
			fread(&offsetTriangles, sizeof(int), 1, fp);
			fread(&offsetFrames, sizeof(int), 1, fp);
			fread(&offsetGlCommands, sizeof(int), 1, fp);
		
			
		// skip to texture coordinates...
		fseek(fp, offsetTexCoords, SEEK_SET);
		model->texCoord = new short*[ model->numTexCoords ];
		for(int i=0; i<model->numTexCoords; i++)
		{
			model->texCoord[i] = new short[2];
			fread(&model->texCoord[i][0], sizeof(short), 1, fp);
			fread(&model->texCoord[i][1], sizeof(short), 1, fp);
		}
			
		// skip to triangle indices...
		fseek(fp, offsetTriangles, SEEK_SET);
		model->triIndex = new short*[ model->numTriangles ];
		model->texIndex = new short*[ model->numTriangles ];
		
		for(i=0; i<model->numTriangles; i++)
		{
			// triangle indices
			model->triIndex[i] = new short[3];
			for(int j=0; j<3; j++)
				fread(&model->triIndex[i][j], sizeof(short), 1, fp);
			
			// texture indices
			model->texIndex[i] = new short[3];
			for(j=0; j<3; j++)
				fread(&model->texIndex[i][j], sizeof(short), 1, fp);

		}
		
		// skip to frame data...
		fseek(fp, offsetFrames, SEEK_SET);
		
		// for each frame...
		model->vertex = new float**[model->numFrames];
		model->normal = new int*[model->numFrames];
		vector<char *> animNameList;
		for(i=0; i<model->numFrames; i++)
		{
			model->vertex[i] = new float*[model->numVertices];
			model->normal[i] = new int[model->numVertices];

			float scale[3];
			fread(&scale[0], sizeof(float), 1, fp);
			fread(&scale[1], sizeof(float), 1, fp);
			fread(&scale[2], sizeof(float), 1, fp);
				
			float translate[3];
			fread(&translate[0], sizeof(float), 1, fp);
			fread(&translate[1], sizeof(float), 1, fp);
			fread(&translate[2], sizeof(float), 1, fp);
			

			char *tempBytes = new char[16];
			fread(tempBytes, 16, sizeof(char), fp);	
			animNameList.push_back(tempBytes);
			
			
			byte tempByte;
			for(int j=0; j<model->numVertices; j++)
			{
				model->vertex[i][j] = new float[3];

				fread(&tempByte, sizeof(byte), 1, fp);
				model->vertex[i][j][0] = (tempByte*scale[0])+translate[0];
				
				fread(&tempByte, sizeof(byte), 1, fp);
				model->vertex[i][j][1] = (tempByte*scale[1])+translate[1];
				
				fread(&tempByte, sizeof(byte), 1, fp);
				model->vertex[i][j][2] = (tempByte*scale[2])+translate[2];

				fread(&tempByte, sizeof(byte), 1, fp);
				model->normal[i][j] = tempByte;
			}
		}

//		for(int h=0; h<animNameList.size(); h++)
//			println("AnimFrame[%i] %s", h, animNameList[h]);
		
		// extract the animation info...
		int animations = 0;
		int currentAnimation = 0;
		for(i=0; i<animNameList.size(); i++)
		{
			// find this name...
			char* tempName = animNameList[i];	

			int index = 0;
			while(tempName[index] > 57 && index < 15)
				index++;
			
			int start = i;	
			char *shouldStartWith = new char[index];
			substr(shouldStartWith, tempName, 1, index);

//			println("StartWith -> %s", shouldStartWith);
			model->animName.push_back(shouldStartWith);	// store the animation name
			
			int thisFrameCount = 0;
			for( ; i<animNameList.size(); i++)
			{
				char *thisFrame = animNameList[i];
				// count the frames...
				char *thisName = new char[index];
				substr(thisName, thisFrame, 1, index);
				if(strcmp(shouldStartWith, thisName) == 0)
				{
					thisFrameCount++;
				}
				else
					break;
				
			}
			
			// i is incremented in the loop so decrement it now...
			model->animStartFrame.push_back(start);
			model->animFrameCount.push_back(i-start);
			i--;
		}
		

	fclose(fp);

	model->setAnimation(0);

	return model;
}


