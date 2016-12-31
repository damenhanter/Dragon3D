#pragma once
#include "datatype.h"
#include "BitmapImage.h"

class CMaterial
{
public:
	CMaterial();
	~CMaterial();
	
public:
	int state;
	int id;
	char name[64];
	int attr;

	RGBAV1 color;      //材质颜色
	float ka,kd,ks,power;    //反射系数和镜面反射指数

	RGBAV1 ra,rd,rs;      //提前得到的颜色和反射系数的积
	char texturePath[80];    //纹理图路径
	CBitmapImage texture;        //纹理图
};