#pragma once
#include "stdafx.h"
#include "terrain.h"

CTerrain::CTerrain()
{
	id=0;
	state=0;
	attr=0;
	nVertices=0;
	nTriangles=0;
	ptrTexture=NULL;
	vlocalList=NULL;
	vtranslist=NULL;
	plist=NULL;
	textlist=NULL;
}
CTerrain::~CTerrain()
{
	if (vlocalList)
	{
		delete [] vlocalList;
		vlocalList=NULL;
	}
	if (vtranslist)
	{
		delete [] vtranslist;
		vtranslist=NULL;
	}
	if (textlist)
	{
		delete [] textlist;
		textlist=NULL;
	}
	if (ptrTexture)
	{
		delete ptrTexture;
		ptrTexture=NULL;
	}
	if (plist)
	{
		delete [] plist;
		plist=NULL;
	}
}

void CTerrain::InitTerrain(int nVertices,int nTriangles)
{
	if ((this->vlocalList=new VERTEX[nVertices])==NULL)
	{
		return ;
	}
	memset(this->vlocalList,0,sizeof(VERTEX)*nVertices);

	if ((this->vtranslist=new VERTEX[nVertices])==NULL)
	{
		return ;
	}
	memset(this->vtranslist,0,sizeof(VERTEX)*nVertices);

	if ((this->textlist=new CPoint2D[nTriangles*3])==NULL)
	{
		return ;
	}
	memset(this->textlist,0,sizeof(CPoint2D)*nTriangles*3);

	if ((plist=new Tri4DV1[nTriangles])==NULL)
	{
		return ;
	}
	memset(plist,0,sizeof(Tri4DV1)*nTriangles);

	this->nTriangles=nTriangles;
	this->nVertices=nVertices;
}

int CTerrain::GenerateTerrain(float twidth,float theight,float vscale,
						CPoint4D& pos,char* heightFieldFile,char* textMapFile,int rgbcolor,int Triattr)
{
	float col_tstep,row_tstep;  //纹理步进因子
	float col_vstep,row_vstep;  //顶点步进因子
	int columns,rows; //顶点列数、顶点行数
	int rgbWhite;
	CBitmapFile bitmapHeight;
	CBitmapFile bitmapText;
	CBitmapImage heightField;

	//1.初始化                             
	state=OBJMODEL4DV1_STATE_ACTIVE|OBJMODEL4DV1_STATE_VISIBLE;
	worldPos=pos;
	rgbWhite=_RGB16BIT565(255,255,255);
	attr=OBJMODEL4DV1_ATTR_SINGLE_FRAME;
	//2.加载高程图
	bitmapHeight.LoadBitmapFromFile(heightFieldFile);

	columns=bitmapHeight.bitmapinfoheader.biWidth;
	rows=bitmapHeight.bitmapinfoheader.biHeight;
	col_vstep=twidth/(float)(columns-1);
	row_vstep=theight/(float)(rows-1);

	ivar1=columns;
	ivar2=rows;
	fvar1=col_vstep;
	fvar2=row_vstep;

	sprintf_s(name,"Terrain:%s,%s",heightFieldFile,textMapFile);
	nVertices=columns*rows;
	nTriangles=(columns-1)*(rows-1)*2;
	//
	//
	InitTerrain(nVertices,nTriangles);
    if (Triattr & TRI4DV1_ATTR_SHADE_MODE_TEXTURE)
    {
		bitmapText.LoadBitmapFromFile(textMapFile);
		ptrTexture=new CBitmapImage;
	    ptrTexture->CreateBitmapImage(0,0,bitmapText.bitmapinfoheader.biWidth,
			bitmapText.bitmapinfoheader.biHeight,bitmapText.bitmapinfoheader.biBitCount);
		ptrTexture->LoadImageBitmap16(bitmapText,0,0,BITMAP_EXTRACT_MODE_ABS);
		col_tstep=(float)(bitmapText.bitmapinfoheader.biWidth-1)/(float)(columns-1);
		row_tstep=(float)(bitmapText.bitmapinfoheader.biHeight-1)/(float)(rows-1);
		SET_BIT(attr,OBJMODEL4DV1_ATTR_TEXTURES);
		bitmapText.UnLoadBitmapFile();
    }
	//4.生成顶点列表和纹理顶点列表
	for(int row=0;row<rows;row++)
	{
		for (int col=0;col<columns;col++)
		{
			int vertex=(row*columns)+col;
			vlocalList[vertex].x=col*col_vstep-twidth/2;
			vlocalList[vertex].y=vscale*((float)bitmapHeight.buffer[(row*columns)+col])/255;
			vlocalList[vertex].z=row*row_vstep-theight/2;
			vlocalList[vertex].w=1;

			SET_BIT(vlocalList[vertex].attr,VERTEX_ATTR_POINT);
			if (Triattr & TRI4DV1_ATTR_SHADE_MODE_TEXTURE)
			{
				textlist[vertex].x=col*col_tstep;
				textlist[vertex].y=row*row_tstep;
			}
		}
	}

	//5.生成多边形列表
	for (int tri=0;tri<nTriangles/2;tri++)
	{
		int index=(tri % (columns-1)) + (columns * (tri / (columns - 1)));
		//左下三角形
		plist[tri*2].vert[0]=index;
		plist[tri*2].vert[1]=index+columns;
		plist[tri*2].vert[2]=index+columns+1;
		//右上三角形
		plist[tri*2+1].vert[0]=index;
		plist[tri*2+1].vert[1]=index+columns+1;
		plist[tri*2+1].vert[2]=index+1;

		plist[tri*2].vlist=vlocalList;
		plist[tri*2+1].vlist=vlocalList;

		plist[tri*2].attr=Triattr;
		plist[tri*2+1].attr=Triattr;

		plist[tri*2].color=rgbcolor;
		plist[tri*2+1].color=rgbcolor;

		//检测是否是GAUOURAD着色
		if ((plist[tri*2].attr & TRI4DV1_ATTR_SHADE_MODE_GOURAUD)||
			(plist[tri*2].attr & TRI4DV1_ATTR_SHADE_MODE_FASTPHONG))
		{
			SET_BIT(vlocalList[plist[tri*2].vert[0]].attr,VERTEX_ATTR_NORMAL);
			SET_BIT(vlocalList[plist[tri*2].vert[1]].attr,VERTEX_ATTR_NORMAL);
			SET_BIT(vlocalList[plist[tri*2].vert[2]].attr,VERTEX_ATTR_NORMAL);

			SET_BIT(vlocalList[plist[tri*2+1].vert[0]].attr,VERTEX_ATTR_NORMAL);
			SET_BIT(vlocalList[plist[tri*2+1].vert[1]].attr,VERTEX_ATTR_NORMAL);
			SET_BIT(vlocalList[plist[tri*2+1].vert[2]].attr,VERTEX_ATTR_NORMAL);
		}

		if (Triattr & TRI4DV1_ATTR_SHADE_MODE_TEXTURE)
		{
			plist[tri*2].ptrTexture=ptrTexture;
			plist[tri*2+1].ptrTexture=ptrTexture;

			plist[tri*2].text[0]=index;
			plist[tri*2].text[1]=index+columns;
			plist[tri*2].text[2]=index+columns+1;

			plist[tri*2+1].text[0]=index;
			plist[tri*2+1].text[1]=index+columns+1;
			plist[tri*2+1].text[2]=index+1;

			plist[tri*2].color=rgbWhite;
			plist[tri*2+1].color=rgbWhite;
			
			SET_BIT(vlocalList[plist[tri*2].vert[0]].attr,VERTEX_ATTR_TEXTURE);
			SET_BIT(vlocalList[plist[tri*2].vert[1]].attr,VERTEX_ATTR_TEXTURE);
			SET_BIT(vlocalList[plist[tri*2].vert[2]].attr,VERTEX_ATTR_TEXTURE);

			SET_BIT(vlocalList[plist[tri*2+1].vert[0]].attr,VERTEX_ATTR_TEXTURE);
			SET_BIT(vlocalList[plist[tri*2+1].vert[1]].attr,VERTEX_ATTR_TEXTURE);
			SET_BIT(vlocalList[plist[tri*2+1].vert[2]].attr,VERTEX_ATTR_TEXTURE);
		}
		SET_BIT(plist[tri*2].attr,TRI4DV1_ATTR_DISABLE_MATERIAL);
		SET_BIT(plist[tri*2+1].attr,TRI4DV1_ATTR_DISABLE_MATERIAL);

		SET_BIT(plist[tri*2].state,TRI4DV1_STATE_ACTIVE);
		SET_BIT(plist[tri*2+1].state,TRI4DV1_STATE_ACTIVE);

		plist[tri*2].vlist=vlocalList;
		plist[tri*2+1].vlist=vlocalList;

		plist[tri*2].textlist=textlist;
		plist[tri*2+1].textlist=textlist;

	}
	ComputeOBJTriLength();
	ComputeOBJVertexNormal();
	
	return true;
}



void CTerrain::ComputeOBJTriLength()
{
	for (int tri=0;tri<nTriangles;tri++)
	{
		int vert0=plist[tri].vert[0];
		int vert1=plist[tri].vert[1];
		int vert2=plist[tri].vert[2];
		CVector4 u,v,n;
		u.CreateVector(vlocalList[vert0].v,vlocalList[vert1].v);
		v.CreateVector(vlocalList[vert0].v,vlocalList[vert2].v);
		n=u.CrossProduct(v);
		plist[tri].nlength=n.GetLength();
	}
}

void CTerrain::ComputeOBJVertexNormal()
{
	int triVertex[MAX_OBJMODEL4DV1_VERTICES];
	memset(triVertex,0,sizeof(int)*MAX_OBJMODEL4DV1_VERTICES);
	for (int tri=0;tri<nTriangles;tri++)
	{
		if (plist[tri].attr & TRI4DV1_ATTR_SHADE_MODE_GOURAUD)
		{
			int vert0=plist[tri].vert[0];
			int vert1=plist[tri].vert[1];
			int vert2=plist[tri].vert[2];
			triVertex[vert0]++;
			triVertex[vert1]++;
			triVertex[vert2]++;

			CVector4 u,v,n;
			u.CreateVector(vlocalList[vert0].v,vlocalList[vert1].v);
			v.CreateVector(vlocalList[vert0].v,vlocalList[vert2].v);
			n=u.CrossProduct(v);

			vlocalList[vert0].n=vlocalList[vert0].n+n;
			vlocalList[vert1].n=vlocalList[vert1].n+n;
			vlocalList[vert2].n=vlocalList[vert2].n+n;
		}
	}
	for (int vert=0;vert<nVertices;vert++)
	{
		if (triVertex[vert]>=1)
		{
			vlocalList[vert].nx/=triVertex[vert];
			vlocalList[vert].ny/=triVertex[vert];
			vlocalList[vert].nz/=triVertex[vert];

			vlocalList[vert].n.Normalize();
		}
	}
}