#pragma once
#include "datatype.h"

#include "RenderSystem.h"

class CTriangle2D
{
public:
	CTriangle2D(void);	
	CTriangle2D(int state, int x0, int y0, int Vx, int Vy);
	CTriangle2D(CTriangle2D& t);
	CTriangle2D& operator=(CTriangle2D& t);
	~CTriangle2D(void);
private:
	void DrawTopTri(int x1, int y1, int x2, int y2, int x3, int y3,int color,UCHAR* destbuffer,int lpitch);
	void DrawBottomTri(int x1, int y1, int x2, int y2, int x3, int y3, int color, UCHAR* destbuffer, int lpitch);
	void DrawTopTri16(int x1, int y1, int x2, int y2, int x3, int y3,int color,UCHAR* destbuffer,int lpitch);
	void DrawBottomTri16(int x1, int y1, int x2, int y2, int x3, int y3, int color, UCHAR* destbuffer, int lpitch);
	void DrawTopTriFP(int x1, int y1, int x2, int y2, int x3, int y3,int color,UCHAR* destbuffer,int lpitch);
	void DrawBottomTriFP(int x1, int y1, int x2, int y2, int x3, int y3, int color, UCHAR* destbuffer, int lpitch);
	void DrawTopTriFP16(int x1, int y1, int x2, int y2, int x3, int y3,int color,UCHAR* destbuffer,int lpitch);
	void DrawBottomTriFP16(int x1, int y1, int x2, int y2, int x3, int y3, int color, UCHAR* destbuffer, int lpitch);
public:
	void DrawTriangle2D(int x1, int y1, int x2, int y2, int x3, int y3, int color , UCHAR* destbuffer, int lpitch);
	void DrawTriangle162D(int x1, int y1, int x2, int y2, int x3, int y3, int color , UCHAR* destbuffer, int lpitch);
	void DrawTriangleFP2D(int x1, int y1, int x2, int y2, int x3, int y3, int color , UCHAR* destbuffer, int lpitch);
	void DrawTriangleFP162D(int x1, int y1, int x2, int y2, int x3, int y3, int color , UCHAR* destbuffer, int lpitch);
private:
	void DrawTopTri16(float x1, float y1, float x2, float y2, float x3, float y3,int color,UCHAR* destbuffer,int lpitch);
	void DrawBottomTri16(float x1, float y1, float x2, float y2, float x3, float y3, int color, UCHAR* destbuffer, int lpitch);
public:	
	void DrawTriangle162D(float x1, float y1, float x2, float y2, float x3, float y3, int color , UCHAR* destbuffer, int lpitch);

//z缓存不支持alpha
public:
	void DrawTriangleZB162D(LPTriF4DV1 face,UCHAR* destbuffer, int lpitch,UCHAR* zBuffer,int zpitch);
	void DrawGouraudTriangleZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch);
	void DrawTextureTriangleZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch);
	void DrawTextureTriangleZBFS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch);
	void DrawTextureTriangleZBGS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch);
	void DrawTextureTriangleBilerpZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch);
//z缓存支持alpha
public:
	void DrawTriangleZBAlpha162D(LPTriF4DV1 face,UCHAR* destbuffer, int lpitch,UCHAR* zBuffer,int zpitch,int alpha);
	void DrawGouraudTriangleZBAlpha16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch,int alpha);
	void DrawTextureTriangleZBAlpha16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch,int alpha);
	void DrawTextureTriangleZBAlphaFS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch,int alpha);
	void DrawTextureTriangleZBAlphaGS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch,int alpha);
//z缓存直接写入
public:
	void DrawTriangleWTZB162D(LPTriF4DV1 face,UCHAR* destbuffer, int lpitch,UCHAR* zBuffer,int zpitch);
	void DrawGouraudTriangleWTZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch);
	void DrawTextureTriangleWTZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch);
	void DrawTextureTriangleWTZBFS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch);
	void DrawTextureTriangleWTZBGS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch);
//1/z缓存不支持alpha
public:
	void DrawTriangleINVZB162D(LPTriF4DV1 face,UCHAR* destbuffer, int lpitch,UCHAR* zBuffer,int zpitch);
	void DrawGouraudTriangleINVZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch);
	void DrawTextureTriangleINVZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch);
	void DrawTextureTriangleINVZBFS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch);
	void DrawTextureTriangleINVZBGS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch);
	void DrawTextureTrianglePerINVZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch);
	void DrawTextureTrianglePerINVZBFS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch);
	void DrawTextureTrianglePerLPINVZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch);
	void DrawTextureTrianglePerLPINVZBFS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch);
	void DrawTextureTriangleBilerpINVZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch);
//1/z缓存支持alpha
public:
	void DrawTriangleINVZBAlpha162D(LPTriF4DV1 face,UCHAR* destbuffer, int lpitch,UCHAR* zBuffer,int zpitch,int alpha);
	void DrawGouraudTriangleINVZBAlpha16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch,int alpha);
	void DrawTextureTriangleINVZBAlpha16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch,int alpha);
	void DrawTextureTriangleINVZBAlphaFS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch,int alpha);
	void DrawTextureTriangleINVZBAlphaGS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch,int alpha);
	void DrawTextureTrianglePerINVZBAlpha16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch,int alpha);
	void DrawTextureTrianglePerINVZBAlphaFS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch,int alpha);
	void DrawTextureTrianglePerLPINVZBAlpha16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch,int alpha);
	void DrawTextureTrianglePerLPINVZBAlphaFS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch,int alpha);
//1/z直接写入
	void DrawTriangleWTINVZB162D(LPTriF4DV1 face,UCHAR* destbuffer, int lpitch,UCHAR* zBuffer,int zpitch);
	void DrawGouraudTriangleWTINVZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch);
	void DrawTextureTriangleWTINVZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch);
	void DrawTextureTriangleWTINVZBFS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch);
	void DrawTextureTriangleWTINVZBGS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch);
	void DrawTextureTriangleWTPerINVZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch);
	void DrawTextureTriangleWTPerINVZBFS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch);
public:
	int CreateMipmaps(CBitmapImage* source,CBitmapImage** mipmaps,float gamma);
protected:
	int state;	// 可见性
	int x0;
	int y0;
	int Vx;
	int Vy;
};
