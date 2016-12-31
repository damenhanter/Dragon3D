#include "stdafx.h"
#include "light.h"

int CLight::CreateLightV1(int state, int attr, RGBAV1 Ca, RGBAV1 Cd, RGBAV1 Cs,
						 CPoint4D &pos, CVector4 &dir, float kc, float kl, float kq,
						 float innerSpot, float outerSpot, int pf,int sp)
{
	this->state=state;
	this->attr=attr;
	this->Ca=Ca;
	this->Cd=Cd;
	this->Cs=Cs;
	this->pos=pos;
	this->tpos=pos;
	this->dir=dir;
	this->tdir=dir;
	this->kc=kc;
	this->kl=kl;
	this->kq=kq;
	this->innerSpot=innerSpot;
	this->outerSpot=outerSpot;
	this->pf=pf;
	this->sp=sp;
	return TRUE;
}

