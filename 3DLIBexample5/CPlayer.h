

class CPlayer
{
public:
	double lookX, lookY, lookZ;
	double posX, posY, posZ;
	double angleX, angleY, angleZ;

	CPlayer(C3Dmodel *theModel, C3DIMAGE2D theSkin);
	~CPlayer();

	void Rotate(float angle, float x, float y, float z);
	void MoveTo(float posX, float posY, float posZ);
	void Move(float speed);
	void Render(void);
	

	C3Dmodel *model;
	C3DIMAGE2D skin;
	double animFrame;
};