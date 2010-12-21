#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>
#include <gl/glut.h>
#include <vector>

using namespace std;

#include <c3dlib.h>
#include <c3dmodel.h>

#include "CPlayer.h"

// APPLICATION SPECIFIC

int rect_x, rect_y; // (x, y) position
int rect_w, rect_h; // width and height

float rect_r; // red color
float rect_g; // green color
float rect_b; // blue color

C3Dlib *gfx;



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



// WINDOWS MESSAGE LOOP AND APPLICATION ENTRY POINT
int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR lpszArgs, int nWinMode)
{
	
	MSG msg;

	gfx = new C3Dlib();

	C3DIMAGE2D marineSkin;

	// Initialise 2DLIB
	gfx->Initialize("3DLIB - Example 3: Walk About", 640, 480, 16, 0, WndProc);
	gfx->LoadBitmap(&marineSkin, "marine.bmp");


	C3Dmodel *myModel = C3Dmodel::loadMD2("marine.md2");
	CPlayer *testPlayer = new CPlayer(myModel, marineSkin);


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
				
				glScalef(0.25, 0.25, 0.25);
				
				testPlayer->render();
			}
			gfx->End();
		}

		// Check for Keyboard Input
		if(gfx->keys[VK_UP])
		{
			testPlayer->move(1);
		}
		if(gfx->keys[VK_RIGHT])
		{
			testPlayer->rotate(0.02);
		}
		if(gfx->keys[VK_LEFT])
		{
			testPlayer->rotate(-0.02);
		}

		
	}

	// cleanup...
	gfx->Shutdown();
	delete gfx;

	return msg.wParam;
}
