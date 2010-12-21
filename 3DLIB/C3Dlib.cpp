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

using namespace std;


C3Dlib::C3Dlib()
{
	hWnd=NULL;
	hRC=NULL;
	hDC=NULL;
	hInstance = NULL;
	fullscreen=TRUE;

	for(int i=0; i<256; i++)
		keys[i] = false;
}

C3Dlib::~C3Dlib()
{
}

BOOL C3Dlib::Initialize(char* title, int width, int height, int bits, bool fullscreenflag, WNDPROC winProc)
{
	GLuint		PixelFormat;
	WNDCLASS	wc;	
	DWORD		dwExStyle;					
	DWORD		dwStyle;

	RECT WindowRect;
	WindowRect.left=(long)0;					
	WindowRect.right=(long)width;
	WindowRect.top=(long)0;		
	WindowRect.bottom=(long)height;	

	fullscreen=fullscreenflag;

	hInstance		= GetModuleHandle(NULL);		
	wc.style		= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= (WNDPROC) winProc;		
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon		= LoadIcon(NULL, IDI_WINLOGO);	
	wc.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= NULL;			
	wc.lpszMenuName		= NULL;	
	wc.lpszClassName	= "OpenGL";	

	if (!RegisterClass(&wc))						// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// Exit And Return FALSE
	}

	if (fullscreen)								// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;					// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));		// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;			// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;			// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;				// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Run In A Window.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","2DLIB",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
					fullscreen=FALSE;				// Select Windowed Mode (Fullscreen=FALSE)
			}
			else
			{
					// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;					// Exit And Return FALSE
			}
		}
	}

	if (fullscreen)								// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;					// Window Extended Style
		dwStyle=WS_POPUP;						// Windows Style
		ShowCursor(FALSE);						// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;					// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	if (!(hWnd=CreateWindowEx(	dwExStyle,				// Extended Style For The Window
					"OpenGL",				// Class Name
					title,					// Window Title
					WS_CLIPSIBLINGS |			// Required Window Style
					WS_CLIPCHILDREN |			// Required Window Style
					dwStyle,				// Selected Window Style
					0, 0,					// Window Position
					WindowRect.right-WindowRect.left,	// Calculate Adjusted Window Width
					WindowRect.bottom-WindowRect.top,	// Calculate Adjusted Window Height
					NULL,					// No Parent Window
					NULL,					// No Menu
					hInstance,				// Instance
					NULL)))
	{
		Shutdown();							// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=					// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),					// Size Of This Pixel Format Descriptor
		1,								// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,						// Must Support Double Buffering
		PFD_TYPE_RGBA,							// Request An RGBA Format
		bits,								// Select Our Color Depth
		0, 0, 0, 0, 0, 0,						// Color Bits Ignored
		0,								// No Alpha Buffer
		0,								// Shift Bit Ignored
		0,								// No Accumulation Buffer
		0, 0, 0, 0,							// Accumulation Bits Ignored
		16,								// 16Bit Z-Buffer (Depth Buffer)
		0,								// No Stencil Buffer
		0,								// No Auxiliary Buffer
		PFD_MAIN_PLANE,							// Main Drawing Layer
		0,								// Reserved
		0, 0, 0								// Layer Masks Ignored
	};

	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		Shutdown();							// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))				// Did Windows Find A Matching Pixel Format?
	{
		Shutdown();							// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))				// Are We Able To Set The Pixel Format?
	{
		Shutdown();							// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))					// Are We Able To Get A Rendering Context?
	{
		Shutdown();							// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))						// Try To Activate The Rendering Context
	{
		Shutdown();							// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							// Return FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);								// Sets Keyboard Focus To The Window
	Resize(width, height);

	// Initialise openGL
	glShadeModel(GL_SMOOTH);

	glEnable(GL_TEXTURE_2D);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	

	glClearDepth(1.0f);							// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);						// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	return TRUE;
}


void C3Dlib::Shutdown(void)
{
	if (fullscreen)								// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);						// Show Mouse Pointer
	}

	if (hRC)								// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))					// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;							// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;							// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;							// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
}

void C3Dlib::Begin(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glLoadIdentity();
}

void C3Dlib::End(void)
{
     glFlush();
	 SwapBuffers(hDC);
}

void C3Dlib::SetMode(int mode)	// C3DMODE_2D | C3DMODE_3D
{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();


	switch(mode)
	{
	case C3DMODE_2D:
		gluOrtho2D(0, width, height, 0);	
		break;

	case C3DMODE_3D:
		gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
		break;
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void C3Dlib::Resize(int width, int height)				
{
	if (height==0)
		height=1;
	glViewport(0, 0, width, height);

	this->width = width;
	this->height = height;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	glOrtho(0, width, height, 0, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


// Primitives
void C3Dlib::Pixel(int x1, int y1, int r, int g, int b)
{
	float fr,fg,fb;

	fr = (float)(r/255.0);
	fg = (float)(g/255.0);
	fb = (float)(b/255.0);

	glBindTexture(GL_TEXTURE_2D, 0);

	glPushMatrix();
	glBegin(GL_POINTS);
	{
		glColor3f(fr,fg,fb);
		glVertex2s(x1,y1);
		glColor3f(1.0,1.0,1.0);
	}
	glEnd();
	glPopMatrix();
}

void C3Dlib::Line(int x1, int y1, int x2, int y2, int r, int g, int b)
{
	float fr,fg,fb;

	fr = (float)(r/255.0);
	fg = (float)(g/255.0);
	fb = (float)(b/255.0);

	glBindTexture(GL_TEXTURE_2D, 0);

	glPushMatrix();
	glBegin(GL_LINES);
	{
		glColor3f(fr,fg,fb);
		glVertex2s(x1,y1);
		glVertex2s(x2,y2);
		glColor3f(1.0,1.0,1.0);
	}
	glEnd();
	glPopMatrix();
}

void C3Dlib::Rect(int x1, int y1, int x2, int y2, int r, int g, int b)
{
	float fr,fg,fb;

	fr = (float)(r/255.0);
	fg = (float)(g/255.0);
	fb = (float)(b/255.0);

	glBindTexture(GL_TEXTURE_2D, 0);

	glPushMatrix();
	glBegin(GL_LINE_STRIP);
	{
		glColor3f(fr,fg,fb);
		glVertex2s(x1,y1);
		glVertex2s(x2,y1);
		glVertex2s(x2,y2);
		glVertex2s(x1,y2);
		glVertex2s(x1,y1);
		glColor3f(1.0,1.0,1.0);
	}
	glEnd();
	glPopMatrix();
}

void C3Dlib::RectFill(int x1, int y1, int x2, int y2, int r, int g, int b)
{
	float fr,fg,fb;

	fr = (float)(r/255.0);
	fg = (float)(g/255.0);
	fb = (float)(b/255.0);

	glBindTexture(GL_TEXTURE_2D, 0);

	glPushMatrix();
	glBegin(GL_QUADS);
	{
		glColor3f(fr,fg,fb);
		glVertex2s(x1,y1);
		glVertex2s(x2,y1);
		glVertex2s(x2,y2);
		glVertex2s(x1,y2);
		glVertex2s(x1,y1);
		glColor3f(1.0,1.0,1.0);
	}
	glEnd();
	glPopMatrix();
}

void C3Dlib::Tri(int x1, int y1, int x2, int y2, int x3, int y3, int r, int g, int b)
{
	float fr,fg,fb;

	fr = (float)(r/255.0);
	fg = (float)(g/255.0);
	fb = (float)(b/255.0);

	glBindTexture(GL_TEXTURE_2D, 0);

	glPushMatrix();
	glBegin(GL_LINE_STRIP);
	{
		glColor3f(fr,fg,fb);
		glVertex2s(x1,y1);
		glVertex2s(x2,y2);
		glVertex2s(x3,y3);
		glVertex2s(x1,y1);
		glColor3f(1.0,1.0,1.0);
	}
	glEnd();
	glPopMatrix();
}

void C3Dlib::TriFill(int x1, int y1, int x2, int y2, int x3, int y3, int r, int g, int b)
{
	float fr,fg,fb;

	fr = (float)(r/255.0);
	fg = (float)(g/255.0);
	fb = (float)(b/255.0);

	glBindTexture(GL_TEXTURE_2D, 0);

	glPushMatrix();
	glBegin(GL_TRIANGLES);
	{
		glColor3f(fr,fg,fb);
		glVertex2s(x1,y1);
		glVertex2s(x2,y2);
		glVertex2s(x3,y3);
		glVertex2s(x1,y1);
		glColor3f(1.0,1.0,1.0);
	}
	glEnd();
	glPopMatrix();
}

// Blitting and Loading Functions
void C3Dlib::LoadBitmap(C3DIMAGE2D *pImage, char *filename)
{
	AUX_RGBImageRec *TextureImage[1];
	memset(TextureImage,0,sizeof(void *)*1);
	char data[262144];

	if (TextureImage[0]=auxDIBImageLoad(filename))
	{

		// Scale the texture to 2^8

		gluScaleImage(GL_RGB, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_UNSIGNED_BYTE, TextureImage[0]->data, 256, 256, GL_UNSIGNED_BYTE, data);


		glGenTextures(1, pImage);	// Create The Texture

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, *pImage);

		glTexImage2D(GL_TEXTURE_2D, 0, 3, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering
	}

	if (TextureImage[0])							// If Texture Exists
	{
		if (TextureImage[0]->data)					// If Texture Image Exists
		{
			free(TextureImage[0]->data);				// Free The Texture Image Memory
		}
		free(TextureImage[0]);						// Free The Image Structure
	}
}


//	NB: Will only work for 24/32 bit uncompressed TGA files
void C3Dlib::LoadTarga(C3DIMAGE2D *pImage, char *filename)					// Loads A TGA File Into Memory
{    
	GLubyte		TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};			// Uncompressed TGA Header
	GLubyte		TGAcompare[12];							// Used To Compare TGA Header
	GLubyte		header[6];							// First 6 Useful Bytes From The Header
	GLuint		bytesPerPixel;							// Holds Number Of Bytes Per Pixel Used In The TGA File
	GLuint		imageSize;							// Used To Store The Image Size When Setting Aside Ram
	GLuint		temp;								// Temporary Variable
	GLuint		type=GL_RGBA;							// Set The Default GL Mode To RBGA (32 BPP)
	i_TgaData   texture[1];
	char data[262144];
	
	FILE *file = fopen(filename, "rb");						// Open The TGA File

	if(	file==NULL ||								// Does File Even Exist?
		fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) ||	// Are There 12 Bytes To Read?
		memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0		||	// Does The Header Match What We Want?
		fread(header,1,sizeof(header),file)!=sizeof(header))			// If So Read Next 6 Header Bytes
	{
		/*if (file == NULL)							// Did The File Even Exist? *Added Jim Strong*
			//return false;							// Return False
		else
		{
			fclose(file);							// If Anything Failed, Close The File
			//return false;							// Return False
		}*/
	}

	texture->width  = header[1] * 256 + header[0];					// Determine The TGA Width	(highbyte*256+lowbyte)
	texture->height = header[3] * 256 + header[2];					// Determine The TGA Height	(highbyte*256+lowbyte)
    
 	if(	texture->width	<=0	||						// Is The Width Less Than Or Equal To Zero
		texture->height	<=0	||						// Is The Height Less Than Or Equal To Zero
		(header[4]!=24 && header[4]!=32))					// Is The TGA 24 or 32 Bit?
	{
		fclose(file);								// If Anything Failed, Close The File
//		return false;								// Return False
	}

	texture->bpp	= header[4];							// Grab The TGA's Bits Per Pixel (24 or 32)
	bytesPerPixel	= texture->bpp/8;						// Divide By 8 To Get The Bytes Per Pixel
	imageSize	= texture->width*texture->height*bytesPerPixel;

	texture->imageData=(GLubyte *)malloc(imageSize);				// Reserve Memory To Hold The TGA Data

	if(	texture->imageData==NULL ||						// Does The Storage Memory Exist?
		fread(texture->imageData, 1, imageSize, file)!=imageSize)		// Does The Image Size Match The Memory Reserved?
	{
		if(texture->imageData!=NULL)						// Was Image Data Loaded
			free(texture->imageData);					// If So, Release The Image Data

		fclose(file);								// Close The File
//		return false;								// Return False
	} 

	for(GLuint i=0; i<int(imageSize); i+=bytesPerPixel)				// Loop Through The Image Data
	{										// Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
		temp=texture->imageData[i];						// Temporarily Store The Value At Image Data 'i'
		texture->imageData[i] = texture->imageData[i + 2];			// Set The 1st Byte To The Value Of The 3rd Byte
		texture->imageData[i + 2] = temp;					// Set The 3rd Byte To The Value In 'temp' (1st Byte Value)
	}

	fclose (file);	

	// Build A Texture From The Data


	gluScaleImage(GL_RGBA, texture->width, texture->height, GL_UNSIGNED_BYTE, texture->imageData, 256, 256, GL_UNSIGNED_BYTE, data);



	glGenTextures(1, pImage);						// Generate OpenGL texture IDs

	glBindTexture(GL_TEXTURE_2D, *pImage);					// Bind Our Texture
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		// Linear Filtered
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		// Linear Filtered

	if (texture[0].bpp==24)								// Was The TGA 24 Bits
	{
		type=GL_RGBA;								// If So Set The 'type' To GL_RGB
	}

	glTexImage2D(GL_TEXTURE_2D, 0, type, 256, 256, 0, type, GL_UNSIGNED_BYTE, data);
}


void C3Dlib::Blit(C3DIMAGE2D *pImage, int x, int y, int w, int h, float rotate)
{
    int half_width = (int)(w/2);
	int half_height = (int)(h/2);

	glBindTexture(GL_TEXTURE_2D, *pImage);

	glPushMatrix();
	glTranslatef((float)x+half_width,(float)y+half_height,0);
	glRotatef(rotate, 0, 0, 1);

	glBegin(GL_QUADS);
	{
		glTexCoord2d(0.0,1.0);glVertex2s(-half_width,-half_height);
		glTexCoord2d(1.0,1.0);glVertex2s(half_width,-half_height);
		glTexCoord2d(1.0,0.0);glVertex2s(half_width,half_width);
		glTexCoord2d(0.0,0.0);glVertex2s(-half_width,half_width);
	}
	glEnd();
	glPopMatrix();
}