#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>
#include <gl/glut.h>
#include <vector>

using namespace std;

#include <c3dlib.h>
#include <c3dmodel.h>
#include <c3dcamera.h>

#include "CPlayer.h"

// APPLICATION SPECIFIC
C3Dlib *gfx;
C3Dcamera *camera;
bool moving = false;
C3DIMAGE2D groundTexture;
C3DIMAGE2D treeTexture;
C3DIMAGE2D wallTexture;

double treePosX[50];
double treePosY[50];

float fogDensity = 0.35f;

// WINDOWS PROCUDURE
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_KEYDOWN:
		{
			gfx->keys[wParam] = TRUE;
			return 0;
		}

		case WM_KEYUP:
		{
			gfx->keys[wParam] = FALSE;
			return 0;
		}

		case WM_SIZE:
		{
			gfx->Resize(LOWORD(lParam),HIWORD(lParam));
			return 0;
		}
	}
	
	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}


void DrawGround()
{
	

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, groundTexture);
	glBegin(GL_QUADS);
	{
		// ground...
		glTexCoord2d(0.0f, 0.0f);
		glVertex3f(-100, 0, -100);

		glTexCoord2d(1.0f, 0.0f);
		glVertex3f(100, 0, -100);

		glTexCoord2d(1.0f, 1.0f);
		glVertex3f(100, 0, 100);

		glTexCoord2d(0.0f, 1.0f);
		glVertex3f(-100, 0, 100);

	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, wallTexture);

	glBegin(GL_QUADS);
	{
		// walls...
		glTexCoord2d(0.0f, 0.0f);
		glVertex3f(-100, 0, -100);

		glTexCoord2d(1.0f, 0.0f);
		glVertex3f(100, 0, -100);

		glTexCoord2d(1.0f, 1.0f);
		glVertex3f(100, 100, -100);

		glTexCoord2d(0.0f, 1.0f);
		glVertex3f(-100, 100, -100);

		glTexCoord2d(0.0f, 0.0f);
		glVertex3f(-100, 0, 100);

		glTexCoord2d(1.0f, 0.0f);
		glVertex3f(100, 0, 100);

		glTexCoord2d(1.0f, 1.0f);
		glVertex3f(100, 100, 100);

		glTexCoord2d(0.0f, 1.0f);
		glVertex3f(-100, 100, 100);

		glTexCoord2d(0.0f, 0.0f);
		glVertex3f(-100, 0, -100);

		glTexCoord2d(1.0f, 0.0f);
		glVertex3f(-100, 0, 100);

		glTexCoord2d(1.0f, 1.0f);
		glVertex3f(-100, 100, 100);

		glTexCoord2d(0.0f, 1.0f);
		glVertex3f(-100, 100, -100);

		glTexCoord2d(0.0f, 0.0f);
		glVertex3f(100, 0, -100);

		glTexCoord2d(1.0f, 0.0f);
		glVertex3f(100, 0, 100);

		glTexCoord2d(1.0f, 1.0f);
		glVertex3f(100, 100, 100);

		glTexCoord2d(0.0f, 1.0f);
		glVertex3f(100, 100, -100);
	}
	glEnd();

	glPopMatrix();
}

void DrawTree()
{
	glBindTexture(GL_TEXTURE_2D, treeTexture);

	glPushMatrix();
	glBegin(GL_QUADS);
	{
		glTexCoord2d(0.0f, 0.0f);
		glVertex3f(-10, 0, 0);

		glTexCoord2d(1.0f, 0.0f);
		glVertex3f(10, 0, 0);

		glTexCoord2d(1.0f, 1.0f);
		glVertex3f(10, 20, 0);

		glTexCoord2d(0.0f, 1.0f);
		glVertex3f(-10, 20, 0);

		glTexCoord2d(0.0f, 0.0f);
		glVertex3f(0, 0, -10);

		glTexCoord2d(1.0f, 0.0f);
		glVertex3f(0, 0, 10);

		glTexCoord2d(1.0f, 1.0f);
		glVertex3f(0, 20, 10);

		glTexCoord2d(0.0f, 1.0f);
		glVertex3f(0, 20, -10);
	}
	glEnd();
	glPopMatrix();


}


// WINDOWS MESSAGE LOOP AND APPLICATION ENTRY POINT
int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR lpszArgs, int nWinMode)
{
	
	MSG msg;

	gfx = new C3Dlib();

	C3DIMAGE2D marineSkin;
	

	// Initialise 3DLIB
	gfx->Initialize("3DLIB - Example 5: Engine Example", 640, 480, 16, 0, WndProc);
	gfx->LoadBitmap(&marineSkin, "marine.bmp");
	gfx->LoadBitmap(&groundTexture, "ground.bmp");
	gfx->LoadBitmap(&wallTexture, "wall.bmp");
	gfx->LoadTarga(&treeTexture, "tree.tga");

	// Set the tree positions (randomly)...
	for(int i=0; i<50; i++)
	{
		treePosX[i] = (rand() % 180) - 90;
		treePosY[i] = (rand() % 180) - 90;
	}


	C3Dmodel *myModel = C3Dmodel::loadMD2("marine.md2");
	CPlayer *testPlayer = new CPlayer(myModel, marineSkin);

	// set the camera...
	camera = new C3Dcamera();

	camera->MoveTo(0.0, 25.0, testPlayer->posZ-40,
				0.0, 0.0, 0.0,
				0.0, 1.0, 0.0);
	testPlayer->MoveTo(camera->viewX, 5, camera->viewZ);

	
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	float fogColor[4] = {0.5f,0.5f,0.5f,1.0f};			// Let's make the Fog Color BLUE

	glFogi(GL_FOG_MODE, GL_LINEAR);		// Fog Mode
	glFogfv(GL_FOG_COLOR, fogColor);			// Set Fog Color
	glFogf(GL_FOG_DENSITY, 0.35f);				// How Dense Will The Fog Be
	glHint(GL_FOG_HINT, GL_DONT_CARE);			// Fog Hint Value
	glFogf(GL_FOG_START, 70.0f);				// Fog Start Depth
	glFogf(GL_FOG_END, 100.0f);				// Fog End Depth
	glEnable(GL_FOG);		
	glClearColor(0.5, 0.5, 0.5, 1.0);

	// Start the Windows Message Loop
	while(msg.message != WM_QUIT)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			
			gfx->Begin();
			{
				// Draw the model...
				gfx->SetMode(C3DMODE_3D);

				camera->Update();

				DrawGround();

				
				for(int i=0; i<50; i++)
				{
					glPushMatrix();
					glTranslatef(treePosX[i], 0, treePosY[i]);
					DrawTree();
					glPopMatrix();
				}
				
				testPlayer->Render();
				
			}
			gfx->End();
		}

		// Check for Keyboard Input
		moving = false;

		if(gfx->keys[VK_UP])
		{
			camera->Move(0.8);
			testPlayer->MoveTo(camera->viewX, 5, camera->viewZ);
			testPlayer->model->setAnimation(1);
			moving = true;
		}
		if(gfx->keys[VK_DOWN])
		{
			camera->Move(0.8);
			testPlayer->MoveTo(camera->viewX, 5, camera->viewZ);
			testPlayer->model->setAnimation(1);
			moving = true;
		}
		if(gfx->keys[VK_LEFT])
		{
			camera->RotateAroundPoint(camera->viewX, camera->viewY, camera->viewZ, 0.01, 0, 1, 0);
			testPlayer->Rotate(0.01, 0, 1, 0);
		}
		if(gfx->keys[VK_RIGHT])
		{
			camera->RotateAroundPoint(camera->viewX, camera->viewY, camera->viewZ, -0.01, 0, 1, 0);
			testPlayer->Rotate(-0.01, 0, 1, 0);
		}

		if(!moving)
			testPlayer->model->setAnimation(0);
		
	}

	// cleanup...
	gfx->Shutdown();
	delete gfx;

	return msg.wParam;
}
