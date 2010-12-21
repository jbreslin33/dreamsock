class CPlayer
{
private:
	double rotation;
	double lookX, lookZ;
	double posX, posZ;
public:
	CPlayer(C3Dmodel *theModel, C3DIMAGE2D theSkin);
	~CPlayer();

	void rotate(double rot);
	void move(int direction);
	void render(void);


	C3Dmodel *model;
	C3DIMAGE2D skin;
	double animFrame;
};