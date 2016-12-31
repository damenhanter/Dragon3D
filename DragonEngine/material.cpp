#include "stdafx.h"
#include "material.h"

CMaterial::CMaterial()
{
	state=0;
	id=0;
	attr=0;
	strcpy(name,"");
	strcpy(texturePath,"");
}

CMaterial::~CMaterial()
{
	texture.Release();
}