extern float frameInterval;

class C3Dcamera
{
public:

	double posX, posY, posZ;
	double viewX, viewY, viewZ;
	double upX, upY, upZ;

	C3Dcamera();	
	
	void MoveTo(float posX, float posY, float posZ,
			 	float viewX,     float viewY,     float viewZ,
				float upX, float upY, float upZ);

	void Rotate(float angle, float x, float y, float z);
	void RotateAroundPoint(double centerX, double centerY, double centerZ, float angle, float x, float y, float z);
	void Move(float speed);
	void Update(void);
};