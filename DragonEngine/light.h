#pragma once
#include "datatype.h"
#include "Vector4.h"

class CLight
{
public:
	CLight()
	{
		state=0;
		id=0;
		attr=0;
		pf=0;
		sp=0;
		kl=kc=kq=0;
		innerSpot=0;
		outerSpot=0;
	}
	int CreateLightV1(int state, int attr, RGBAV1 Ca, RGBAV1 Cd, RGBAV1 Cs,
		CPoint4D &pos, CVector4 &dir, float kc, float kl, float kq,
		float innerSpot, float outerSpot, int pf,int sp);
public:
	int state;
	int id;
	int attr;

	RGBAV1 Ca;        //环境光强度
	RGBAV1 Cd;        //漫反射光强度
	RGBAV1 Cs;        //镜面光强度
	CPoint4D pos;     //光源位置
	CPoint4D tpos;
	CVector4 dir;     //光源方向
	CVector4 tdir;
	float kc,kl,kq;   //衰减因子
	float innerSpot;   //聚光灯内锥角
	float outerSpot;   //聚光灯外锥角
	int pf;          //聚光灯指数因子
	int sp;           //镜面反射指数
};