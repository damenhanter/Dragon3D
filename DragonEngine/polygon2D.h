#pragma once
#include "datatype.h"
#include "Line2D.h"

class CPolygon2D
{
public:
	CPolygon2D(void);
	CPolygon2D(int state, int numVerts, POINT centerPoint,VERTEX2D* pVertex, int Vx, int Vy, int color);
	~CPolygon2D(void);
public:
	int DrawPolygon2D(UCHAR* vbuffer, int lpitch);
	int DrawPolygon162D(UCHAR* vbuffer, int lpitch);
	int TranslatePolygon2D(int dx, int dy);	// 平移多边形
	int RotatePolygon2D(int angle);
	int ScalePolygon2D(float sx, float sy);
	int DrawFillPolygon2D(UCHAR* vbuffer, int lpitch);	// 多边形填充
	int DrawFillPolygon162D(UCHAR* vbuffer, int lpitch);
public:
	LPVERTEX2D m_pVertex;
	int m_NumVerts;	           //定点数量
	int m_State;               //多边形显示状态
	POINT m_CenterPoint;       //多边形位置
	int m_Vx;                  // X方向速度
	int m_Vy;                  // Y 方向速度
	DWORD m_Color;
	CLine2D line2D;
};
