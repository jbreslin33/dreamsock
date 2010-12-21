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


void DrawGrid()
{
	glBindTexture(GL_TEXTURE_2D, 0);

	glPushMatrix();
	glBegin(GL_LINES);
	{
		glColor3f(0.0, 1.0, 0.0);
		for(int i=-100; i<=100; i+=2)
		{
			glVertex3f(i, 1, -100);
			glVertex3f(i, 0, 100);

			glVertex3f(-100, 0, i);
			glVertex3f(100, 0, i);
		}
		glColor3f(1.0, 1.0, 1.0);
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
	gfx->Initialize("3DLIB - Example 4: Grid Walker", 640, 480, 16, 0, WndProc);
	gfx->LoadBitmap(&marineSkin, "marine.bmp");


	C3Dmodel *myModel = C3Dmodel::loadMD2("marine.md2");
	CPlayer *testPlayer = new CPlayer(myModel, marineSkin);

	// set the camera...
	camera = new C3Dcamera();

	camera->MoveTo(0.0, 25.0, testPlayer->posZ-40,
				0.0, 0.0, 0.0,
				0.0, 1.0, 0.0);
	testPlayer->MoveTo(camera->viewX, 5, camera->viewZ);

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

				DrawGrid();

				testPlayer->Render();
				
			}
			gfx->End();
		}

		// Check for Keyboard Input
		moving = false;

		if(gfx->keys[VK_UP])
		{
			camera->Move(0.005);
			testPlayer->MoveTo(camera->viewX, 5, camera->viewZ);
			testPlayer->model->setAnimation(1);
			moving = true;
		}
		if(gfx->keys[VK_DOWN])
		{
			camera->Move(-0.005);
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
