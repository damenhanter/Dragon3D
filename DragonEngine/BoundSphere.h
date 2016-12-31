#pragma once
#include <Windows.h>

class CBoundSphere 
{
public:
	CBoundSphere(void);
	~CBoundSphere(void);
	int CreateColliCheck(POINT point[],int NumVerts);
	int CreateColliCheck(int x0,int y0,int radius);
	int IsPointInside(int x, int y);
	BOOL IsCollision(CBoundSphere* Bounding);
private:
	int x0,y0,radius;
public:
	int Distance2D(int x, int y);
	float Distance3D(float x, float y, float z);
};
