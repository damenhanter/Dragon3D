#pragma once
#include "datatype.h"
extern RECT g_rRectClip;

class CLine2D
{
public:
	CLine2D(void);
	~CLine2D(void);
	int DrawLine2D(int x1, int y1, int x2, int y2, UCHAR color, UCHAR* vb_start, int lpitch);// 画直线
	int DrawLine162D(int x1, int y1, int x2, int y2, int color, UCHAR* vb_start, int lpitch);
	int DrawClipLine2D(int x1, int y1, int x2, int y2, int color, UCHAR* vb_start, int lpitch);
	int DrawClipLine162D(int x1, int y1, int x2, int y2, int color, UCHAR* vb_start, int lpitch);
private:
	int ClipLine2D(int& x1, int& y1, int& x2, int& y2);
	POINT ClipMid2D(int x1, int y1, int x2, int y2);
    int IsInArea2D(int x,int y);
	
	//直线与窗口不相交
	int NotIntersect2D(int x1,int y1,int x2,int y2);
};
