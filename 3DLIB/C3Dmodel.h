class C3Dmodel
{
public:
	short** texCoord;	// coordinate | 0=u, 1=v
	float*** vertex; // frame | vertex | 0=x,1=y,2=z
	int** normal;	// frame | vertex
	short** triIndex;	// indices into the 'vertex' part of the vertex array...
	short** texIndex;	// indices into the 'textCoord' array - triangle | index
	int skinWidth, skinHeight;
	int numSkins, numVertices, numTexCoords, numTriangles, numGlCommands, numFrames;

	// Animation data...
	int currentAnimation;
	vector<char *> animName;
	vector<int> animStartFrame;
	vector<int> animFrameCount;

	void render();
	void render(int frame);
	void render(double time);
	bool setAnimation(int id);
	
	char *getAnimationName();

	static C3Dmodel *loadMD2(char *filename);
};