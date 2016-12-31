#pragma once
#include "polygon2D.h"

class CBoundBox 
{
public:
	CBoundBox(void);
	~CBoundBox(void);
	int CreateColliCheck(CPolygon2D* polygon2D);
	int IsPointInside(int x, int y);
	BOOL IsCollision(CBoundBox* Bounding);
private:
	int xmin,xmax,ymin,ymax;
};
