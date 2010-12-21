// Definitions
#define GFX_IMAGE2D unsigned int

// Initialisation
BOOL GFX_Init(char* title, int width, int height, int bits, bool fullscreenflag, WNDPROC winProc);
void GFX_Shutdown(void);
void GFX_Begin(void);
void GFX_End(void);
void GFX_Resize(int width, int height);

// Primitives
void GFX_Pixel(int x1, int y1, int r, int g, int b);
void GFX_Line(int x1, int y1, int x2, int y2, int r, int g, int b);
void GFX_Rect(int x1, int y1, int x2, int y2, int r, int g, int b);
void GFX_RectFill(int x1, int y1, int x2, int y2, int r, int g, int b);
void GFX_Tri(int x1, int y1, int x2, int y2, int x3, int y3, int r, int g, int b);
void GFX_TriFill(int x1, int y1, int x2, int y2, int x3, int y3, int r, int g, int b);

// Blitting
void GFX_LoadBitmap(GFX_IMAGE2D *pImage, char *filename);		
void GFX_LoadTarga(GFX_IMAGE2D *pImage, char *filename);		
void GFX_Blit(GFX_IMAGE2D *pImage, int x, int y, int w, int h, float rotate);

// Definitions
#define GFX_IMAGE2D unsigned int

// Structures
typedef	struct										// Create A Structure
{
	GLubyte	*imageData;								// Image Data (Up To 32 Bits)
	GLuint	bpp;									// Image Color Depth In Bits Per Pixel
	GLuint	width;									// Image Width
	GLuint	height;									// Image Height
	GLuint	texID;									// Texture ID Used To Select A Texture
} i_TgaData;

// Externs
extern HWND			hWnd;
extern HGLRC		hRC;
extern HDC			hDC;
extern HINSTANCE    hInstance;

extern bool	keys[256];
extern bool	fullscreen;