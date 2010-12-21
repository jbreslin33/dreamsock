#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>
#include <vector>

using namespace std;

#include <c3dlib.h>
#include <c3dmodel.h>

// APPLICATION SPECIFIC

int rect_x, rect_y; // (x, y) position
int rect_w, rect_h; // width and height

float rect_r; // red color
float rect_g; // green color
float rect_b; // blue color

C3Dlib *gfx;

double rot = 0.0;
double animFrame = 0.0;


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
	gfx->Initialize("3DLIB - Example 2: MD2 Models", 640, 480, 16, 0, WndProc);
	gfx->LoadBitmap(&marineSkin, "marine.bmp");


	C3Dmodel *myModel = C3Dmodel::loadMD2("marine.md2");
	myModel->setAnimation(0);

	// Setup the Rectangle
	rect_x = 10;  // x position is 10
	rect_y = 10;  // y position is 10
	rect_w = 100;  // width is 100 pixels
	rect_h = 100; // height is 100 pixels

	rect_r = 255.0; // red is full intensity
	rect_g = 0.0;   // no blue
	rect_b = 0.0;   // no green

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
				
				glTranslatef(0.0, 0.0,-25);
				glRotatef(90, 1, 0, 0);
				glRotatef(180, 0, 1, 0);

				glRotatef(rot, 0, 0, 1);

				glScalef(0.25, 0.25, 0.25);
				glBindTexture(GL_TEXTURE_2D, marineSkin);
				myModel->render(animFrame);
			}
			gfx->End();
		}

		// Check for Keyboard Input
		if(gfx->keys[VK_RIGHT])
		{
			rot -= 1.5;
			if(rot > 360)
				 rot -= 360;
		}
		if(gfx->keys[VK_LEFT])
		{
			rot += 1.5;
			if(rot < 0)
				 rot += 360;
		}

		// -> Colors
		if(gfx->keys[VkKeyScan('r')])
		{
			rect_r = 255.0;
			rect_g = 0.0;
			rect_b = 0.0;
		}
		if(gfx->keys[VkKeyScan('g')])
		{
			rect_r = 0.0;
			rect_g = 255.0;
			rect_b = 0.0;
		}
		if(gfx->keys[VkKeyScan('b')])
		{
			rect_r = 0.0;
			rect_g = 0.0;
			rect_b = 255.0;
		}

		animFrame += 0.005;
		if(animFrame > 1)
			animFrame -= 1;
	}

	// cleanup...
	gfx->Shutdown();
	delete gfx;

	return msg.wParam;
}
