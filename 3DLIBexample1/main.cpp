#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>
#include <gl/glut.h>
#include <c3dlib.h>

// APPLICATION SPECIFIC

int rect_x, rect_y; // (x, y) position
int rect_w, rect_h; // width and height

float rect_r; // red color
float rect_g; // green color
float rect_b; // blue color

C3Dlib *gfx;

double rot = 0.0;


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

	// Initialise 2DLIB
	gfx->Initialize("3DLIB - Example 1: 2D Rendering", 640, 480, 16, 0, WndProc);


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
				// Draw the Rectangle
				gfx->SetMode(C3DMODE_2D);
				
				gfx->RectFill(rect_x, rect_y, rect_x+rect_w, rect_y+rect_h, rect_r, rect_g, rect_b);
				gfx->SetMode(C3DMODE_3D);
				
				glTranslatef(0.0, 0.0,-5);
				glRotatef(rot, 1, 1, 1);
				glutWireCube(1.0f);

			

				
			}
			gfx->End();
		}

		// Check for Keyboard Input

		// -> Movement
		if(gfx->keys[VK_UP])
		{
			if(rect_y > 0)
				rect_y--;
		}
		if(gfx->keys[VK_DOWN])
		{
			if(rect_y+rect_h < 480)
				rect_y++;
		}
		if(gfx->keys[VK_RIGHT])
		{
			if(rect_x+rect_w < 640)
				rect_x++;
		}
		if(gfx->keys[VK_LEFT])
		{
			if(rect_x > 0)
				rect_x--;
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

		rot += 0.3;
		if(rot > 1000)
			rot -= 1000;
	}

	// cleanup...
	gfx->Shutdown();
	delete gfx;

	return msg.wParam;
}
