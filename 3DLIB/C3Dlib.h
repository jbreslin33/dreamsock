// Definitions
#define C3DIMAGE2D unsigned int
#define C3DMODE_2D 0
#define C3DMODE_3D 1

class C3Dlib
{
private:
	HWND		hWnd;
	HGLRC		hRC;
	HDC			hDC;
	HINSTANCE   hInstance;
	bool	fullscreen;
	int width, height;

public:
	C3Dlib();
	~C3Dlib();

	BOOL Initialize(char* title, int width, int height, int bits, bool fullscreenflag, WNDPROC winProc);
	void Shutdown(void);
	void Begin(void);
	void End(void);
	void Resize(int width, int height);
	void SetMode(int mode);

	// Primitives
	void Pixel(int x1, int y1, int r, int g, int b);
	void Line(int x1, int y1, int x2, int y2, int r, int g, int b);
	void Rect(int x1, int y1, int x2, int y2, int r, int g, int b);
	void RectFill(int x1, int y1, int x2, int y2, int r, int g, int b);
	void Tri(int x1, int y1, int x2, int y2, int x3, int y3, int r, int g, int b);
	void TriFill(int x1, int y1, int x2, int y2, int x3, int y3, int r, int g, int b);

	void LoadBitmap(C3DIMAGE2D *pImage, char *filename);		
	void LoadTarga(C3DIMAGE2D *pImage, char *filename);		
	void Blit(C3DIMAGE2D *pImage, int x, int y, int w, int h, float rotate);

	bool	keys[256];
};

// Structures
typedef	struct										// Create A Structure
{
	GLubyte	*imageData;								// Image Data (Up To 32 Bits)
	GLuint	bpp;									// Image Color Depth In Bits Per Pixel
	GLuint	width;									// Image Width
	GLuint	height;									// Image Height
	GLuint	texID;									// Texture ID Used To Select A Texture
} i_TgaData;

