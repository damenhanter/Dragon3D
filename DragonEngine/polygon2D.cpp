
#include "stdafx.h"
#include "polygon2D.h"

extern RECT g_rRectClip;
extern float cosTable[361];
extern float sinTable[361];

CPolygon2D::CPolygon2D(void)
: m_State(0)
, m_NumVerts(0)
, m_Vx(0)
, m_Vy(0)
, m_Color(0)
{
	m_CenterPoint=POINT();
	m_pVertex=NULL;
}

CPolygon2D::~CPolygon2D(void)
{
	if(m_pVertex!=NULL)
	{
		delete m_pVertex;
		m_pVertex=NULL;
	}
}

CPolygon2D::CPolygon2D(int state, int numVerts, POINT centerPoint,VERTEX2D* pVertex, int Vx, int Vy, int color)
{
	m_State=state;
	m_NumVerts=numVerts;
	m_CenterPoint=centerPoint;
	m_Vx=Vx;
	m_Vy=Vy;
	m_Color=color;
	m_pVertex=new VERTEX2D[numVerts];
	for(int i=0; i<numVerts; i++)
	{
		m_pVertex[i]=pVertex[i];
	}
}

int CPolygon2D::DrawPolygon2D(UCHAR* vbuffer, int lpitch)
{
	if(m_State)
	{
		POINT BPoint,EPoint;
		int index;
		for(index=0; index<m_NumVerts-1; index++)
		{
			BPoint.x=long(m_pVertex[index].x+m_Vx);
			BPoint.y=long(m_pVertex[index].y+m_Vy);
			EPoint.x=long(m_pVertex[index+1].x+m_Vx);
			EPoint.y=long(m_pVertex[index+1].y+m_Vy);
			line2D.DrawClipLine2D(BPoint.x,BPoint.y,EPoint.x,EPoint.y,
				(UCHAR)m_Color,vbuffer,lpitch);
		}
		BPoint.x=long(m_pVertex[index].x+m_Vx);
		BPoint.y=long(m_pVertex[index].y+m_Vy);
		EPoint.x=long(m_pVertex[0].x+m_Vx);
		EPoint.y=long(m_pVertex[0].y+m_Vy);
		line2D.DrawClipLine2D(BPoint.x,BPoint.y,EPoint.x,EPoint.y,
				(UCHAR)m_Color,vbuffer,lpitch);
		return true;
	}
	else
		return false;
}
int CPolygon2D::DrawPolygon162D(UCHAR* vbuffer, int lpitch)
{
	if(m_State)
	{
		POINT BPoint,EPoint;
		int index;
		for(index=0; index<m_NumVerts-1; index++)
		{
			BPoint.x=long(m_pVertex[index].x+m_Vx);
			BPoint.y=long(m_pVertex[index].y+m_Vy);
			EPoint.x=long(m_pVertex[index+1].x+m_Vx);
			EPoint.y=long(m_pVertex[index+1].y+m_Vy);
			line2D.DrawClipLine162D(BPoint.x,BPoint.y,EPoint.x,EPoint.y,
				(UCHAR)m_Color,vbuffer,lpitch);
		}
		BPoint.x=long(m_pVertex[index].x+m_Vx);
		BPoint.y=long(m_pVertex[index].y+m_Vy);
		EPoint.x=long(m_pVertex[0].x+m_Vx);
		EPoint.y=long(m_pVertex[0].y+m_Vy);
		line2D.DrawClipLine162D(BPoint.x,BPoint.y,EPoint.x,EPoint.y,
				(UCHAR)m_Color,vbuffer,lpitch);
		return true;
	}
	else
		return false;
}


// 平移多边形
int CPolygon2D::TranslatePolygon2D(int dx, int dy)
{
	m_CenterPoint.x+=dx;
	m_CenterPoint.y+=dy;
	return true;
}

int CPolygon2D::RotatePolygon2D(int angle)
{
	for(int currVert=0; currVert<m_NumVerts; currVert++)
	{
		float xr=m_pVertex[currVert].x*cosTable[angle]-
			m_pVertex[currVert].y*sinTable[angle];
		float yr=m_pVertex[currVert].x*sinTable[angle]+
			m_pVertex[currVert].y*cosTable[angle];
		m_pVertex[currVert].x=xr;
		m_pVertex[currVert].y=yr;
	}
	return true;
}

int CPolygon2D::ScalePolygon2D(float sx, float sy)
{
	for(int currVert=0; currVert<m_NumVerts; currVert++)
	{
		m_pVertex[currVert].x*=sx;
		m_pVertex[currVert].y*=sy;
	}
	return true;
}

// 多边形填充
int CPolygon2D::DrawFillPolygon2D(UCHAR* vbuffer, int lpitch)
{
	int xdiff1,ydiff1,xdiff2,ydiff2,start,length;
	int errorterm1,errorterm2,offset1,offset2,count1,count2,xunit1,xunit2;
	int edgecount=m_NumVerts-1;
	int firstvert=0;
	int miny=(int)m_pVertex[0].y;
	for(int i=1; i<m_NumVerts; i++)
	{
		if(m_pVertex[i].y<miny)
		{
			firstvert=i;
			miny=(int)m_pVertex[i].y;
		}
	}
	int startvert1=firstvert;
	int startvert2=firstvert;
	int xstart1=(int)m_pVertex[startvert1].x+m_CenterPoint.x;
	int ystart1=(int)m_pVertex[startvert1].y+m_CenterPoint.y;
	int xstart2=(int)m_pVertex[startvert2].x+m_CenterPoint.x;
	int ystart2=(int)m_pVertex[startvert2].y+m_CenterPoint.y;
	int endvert1=startvert1-1;
	if(endvert1<0)
		endvert1=m_NumVerts-1;
	int xend1=(int)m_pVertex[endvert1].x+m_CenterPoint.x;
	int yend1=(int)m_pVertex[endvert1].y+m_CenterPoint.y;
	int endvert2=startvert2+1;
	if(endvert2==m_NumVerts)
		endvert2=0;
	int xend2=(int)m_pVertex[endvert2].x+m_CenterPoint.x;
	int yend2=(int)m_pVertex[endvert2].y+m_CenterPoint.y;
	while(edgecount>0)
	{
		offset1=lpitch*ystart1+xstart1;
		offset2=lpitch*ystart2+xstart2;
		errorterm1=0;
		errorterm2=0;
		if((ydiff1=yend1-ystart1)<0)
			ydiff1=-ydiff1;
		if((ydiff2=yend2-ystart2)<0)
			ydiff2=-ydiff2;
		if((xdiff1=xend1-xstart1)<0)
		{
			xunit1=-1;
			xdiff1=-xdiff1;
		}
		else
			xunit1=1;
		if((xdiff2=xend2-xstart2)<0)
		{
			xunit2=-1;
			xdiff2=-xdiff2;
		}
		else
			xunit2=1;
		if(xdiff1>ydiff1)
		{
			if(xdiff2>ydiff2)
			{
				count1=xdiff1;          //caculate x increment for edge1
				count2=xdiff2;          //caculate x increment for edge2
				while(count1 && count2)
				{
					while((errorterm1<xdiff1) && (count1>0))
					{
						if(count1--)
						{
							offset1+=xunit1;
							xstart1+=xunit1;
						}
						errorterm1+=ydiff1;
						if(errorterm1<xdiff1)
						{
							vbuffer[offset1]=(UCHAR)m_Color;
						}
					}
					errorterm1-=xdiff1;
					while((errorterm2<xdiff2) && (count2>0))
					{
						if(count2--)
						{
							offset2+=xunit2;
							xstart2+=xunit2;
						}
						errorterm2+=ydiff2;
						if(errorterm2<xdiff2)
						{
							vbuffer[offset2]=(UCHAR)m_Color;
						}
					}
					errorterm2-=xdiff2;
					length=offset2-offset1;
					if(length<0)
					{
						length=-length;
						start=offset2;
					}
					else
						start=offset1;
					for(int i=start; i<start+length+1; i++)
						vbuffer[i]=(UCHAR)m_Color;
					offset1+=lpitch;
					ystart1++;
					offset2+=lpitch;
					ystart2++;
				}
			} //end if
			else
			{
				count1=xdiff1;          //caculate x increment for edge1
				count2=ydiff2;          //caculate y increment for edge2
				while(count1 && count2)
				{
					while((errorterm1<xdiff1) && (count1>0))
					{
						if(count1--)
						{
							offset1+=xunit1;
							xstart1+=xunit1;
						}
						errorterm1+=ydiff1;
						if(errorterm1<xdiff1)
						{
							vbuffer[offset1]=(UCHAR)m_Color;
						}
					}
					errorterm1-=xdiff1;
					// caculate edge 2
					errorterm2+=xdiff2;
					if(errorterm2>=ydiff2)
					{
						errorterm2-=ydiff2;
						offset2+=xunit2;
						xstart2+=xunit2;
					}
					count2--;
					length=offset2-offset1;
					if(length<0)
					{
						length=-length;
						start=offset2;
					}
					else
						start=offset1;
					for(int i=start; i<start+length+1; i++)
						vbuffer[i]=(UCHAR)m_Color;
					offset1+=lpitch;
					ystart1++;
					offset2+=lpitch;
					ystart2++;
				}
			}
		}
		else
		{
			if(xdiff2>ydiff2)
			{
				count1=ydiff1;  //calculate y increment for edge 1
				count2=xdiff2;  //calculate x increment for edge 2
				while(count1 && count2)
				{
					errorterm1+=xdiff1;
					if(errorterm1>=ydiff1)
					{
						errorterm1-=ydiff1;
						offset1+=xunit1;
						xstart1+=xunit1;
					}
					count1--;
					while((errorterm2<xdiff2) && (count2>0))
					{
						if(count2--)
						{
							offset2+=xunit2;
							xstart2+=xunit2;
						}
						errorterm2+=ydiff2;
						if(errorterm2<xdiff2)
						{
							vbuffer[offset2]=(UCHAR)m_Color;
						}
					}
					errorterm2-=xdiff2;
					length=offset2-offset1;
					if(length<0)
					{
						length=-length;
						start=offset2;
					}
					else 
					{
						start=offset1;
					}
					for(int i=start; i<start+length+1; i++)
						vbuffer[i]=(UCHAR)m_Color;
					offset1+=lpitch;
					ystart1++;
					offset2+=lpitch;
					ystart2++;
				}
			}
			else
			{
				count1=ydiff1;          //calculate y increment for edge1
				count2=ydiff2;          //calculate y increment for edge2
				while(count1 && count2)
				{
					errorterm1+=xdiff1;
					if(errorterm1>=ydiff1)
					{
						errorterm1-=ydiff1;
						offset1+=xunit1;
						xstart1+=xunit1;
					}
					count1--;
					// calculate edge 2
					errorterm2+=xdiff2;
					if(errorterm2>=ydiff2)
					{
						errorterm2-=ydiff2;
						offset2+=xunit2;
						xstart2+=xunit2;
					}
					--count2;
					length=offset2-offset1;
					if(length<0)
					{
						length=-length;
						start=offset2;
					}
					else
						start=offset1;
					for(int i=start; i<start+length+1; i++)
						vbuffer[i]=(UCHAR)m_Color;
					offset1+=lpitch;
					ystart1++;
					offset2+=lpitch;
					ystart2++;
				}//end while
			}//end else
		}//end else
		if(!count1)
		{
			--edgecount;
			startvert1=endvert1;
			--endvert1;

			if(endvert1<0)
				endvert1=m_NumVerts-1;
			xend1=(int)m_pVertex[endvert1].x+m_CenterPoint.x;
			yend1=(int)m_pVertex[endvert1].y+m_CenterPoint.y;
		}
		if(!count2)
		{
			--edgecount;
			startvert2=endvert2;
			endvert2++;

			if(endvert2==m_NumVerts)
				endvert2=0;
			xend2=(int)m_pVertex[endvert2].x+m_CenterPoint.x;
			yend2=(int)m_pVertex[endvert2].y+m_CenterPoint.y;
		}
	}
	return true;
}

int CPolygon2D::DrawFillPolygon162D(UCHAR* vbuffer, int lpitch)
{
	int xdiff1,ydiff1,xdiff2,ydiff2,start,length;
	int errorterm1,errorterm2,offset1,offset2,count1,count2,xunit1,xunit2;
	int edgecount=m_NumVerts-1;
	int firstvert=0;
	lpitch>>=1;
	USHORT* vbuffer16=(USHORT*)vbuffer;
	int miny=(int)m_pVertex[0].y;
	for(int i=1; i<m_NumVerts; i++)
	{
		if(m_pVertex[i].y<miny)
		{
			firstvert=i;
			miny=(int)m_pVertex[i].y;
		}
	}
	int startvert1=firstvert;
	int startvert2=firstvert;
	int xstart1=(int)m_pVertex[startvert1].x+m_CenterPoint.x;
	int ystart1=(int)m_pVertex[startvert1].y+m_CenterPoint.y;
	int xstart2=(int)m_pVertex[startvert2].x+m_CenterPoint.x;
	int ystart2=(int)m_pVertex[startvert2].y+m_CenterPoint.y;
	int endvert1=startvert1-1;
	if(endvert1<0)
		endvert1=m_NumVerts-1;
	int xend1=(int)m_pVertex[endvert1].x+m_CenterPoint.x;
	int yend1=(int)m_pVertex[endvert1].y+m_CenterPoint.y;
	int endvert2=startvert2+1;
	if(endvert2==m_NumVerts)
		endvert2=0;
	int xend2=(int)m_pVertex[endvert2].x+m_CenterPoint.x;
	int yend2=(int)m_pVertex[endvert2].y+m_CenterPoint.y;
	while(edgecount>0)
	{
		offset1=lpitch*ystart1+xstart1;
		offset2=lpitch*ystart2+xstart2;
		errorterm1=0;
		errorterm2=0;
		if((ydiff1=yend1-ystart1)<0)
			ydiff1=-ydiff1;
		if((ydiff2=yend2-ystart2)<0)
			ydiff2=-ydiff2;
		if((xdiff1=xend1-xstart1)<0)
		{
			xunit1=-1;
			xdiff1=-xdiff1;
		}
		else
			xunit1=1;
		if((xdiff2=xend2-xstart2)<0)
		{
			xunit2=-1;
			xdiff2=-xdiff2;
		}
		else
			xunit2=1;
		if(xdiff1>ydiff1)
		{
			if(xdiff2>ydiff2)
			{
				count1=xdiff1;          //calculate x increment for edge1
				count2=xdiff2;          //calculate x increment for edge2
				while(count1 && count2)
				{
					while((errorterm1<xdiff1) && (count1>0))
					{
						if(count1--)
						{
							offset1+=xunit1;
							xstart1+=xunit1;
						}
						errorterm1+=ydiff1;
						if(errorterm1<xdiff1)
						{
							vbuffer16[offset1]=(USHORT)m_Color;
						}
					}
					errorterm1-=xdiff1;
					while((errorterm2<xdiff2) && (count2>0))
					{
						if(count2--)
						{
							offset2+=xunit2;
							xstart2+=xunit2;
						}
						errorterm2+=ydiff2;
						if(errorterm2<xdiff2)
						{
							vbuffer16[offset2]=(USHORT)m_Color;
						}
					}
					errorterm2-=xdiff2;
					length=offset2-offset1;
					if(length<0)
					{
						length=-length;
						start=offset2;
					}
					else
						start=offset1;
					for(int i=start; i<start+length+1; i++)
						vbuffer16[i]=(USHORT)m_Color;
					offset1+=lpitch;
					ystart1++;
					offset2+=lpitch;
					ystart2++;
				}
			} //end if
			else
			{
				count1=xdiff1;          //calculate x increment for edge1
				count2=ydiff2;          //calculate y increment for edge2
				while(count1 && count2)
				{
					while((errorterm1<xdiff1) && (count1>0))
					{
						if(count1--)
						{
							offset1+=xunit1;
							xstart1+=xunit1;
						}
						errorterm1+=ydiff1;
						if(errorterm1<xdiff1)
						{
							vbuffer16[offset1]=(USHORT)m_Color;
						}
					}
					errorterm1-=xdiff1;
					// calculate edge 2
					errorterm2+=xdiff2;
					if(errorterm2>=ydiff2)
					{
						errorterm2-=ydiff2;
						offset2+=xunit2;
						xstart2+=xunit2;
					}
					count2--;
					length=offset2-offset1;
					if(length<0)
					{
						length=-length;
						start=offset2;
					}
					else
						start=offset1;
					for(int i=start; i<start+length+1; i++)
						vbuffer16[i]=(USHORT)m_Color;
					offset1+=lpitch;
					ystart1++;
					offset2+=lpitch;
					ystart2++;
				}
			}
		}
		else
		{
			if(xdiff2>ydiff2)
			{
				count1=ydiff1;  //calculate y increment for edge 1
				count2=xdiff2;  //calculate x increment for edge 2
				while(count1 && count2)
				{
					errorterm1+=xdiff1;
					if(errorterm1>=ydiff1)
					{
						errorterm1-=ydiff1;
						offset1+=xunit1;
						xstart1+=xunit1;
					}
					count1--;
					while((errorterm2<xdiff2) && (count2>0))
					{
						if(count2--)
						{
							offset2+=xunit2;
							xstart2+=xunit2;
						}
						errorterm2+=ydiff2;
						if(errorterm2<xdiff2)
						{
							vbuffer16[offset2]=(USHORT)m_Color;
						}
					}
					errorterm2-=xdiff2;
					length=offset2-offset1;
					if(length<0)
					{
						length=-length;
						start=offset2;
					}
					else 
					{
						start=offset1;
					}
					for(int i=start; i<start+length+1; i++)
						vbuffer16[i]=(USHORT)m_Color;
					offset1+=lpitch;
					ystart1++;
					offset2+=lpitch;
					ystart2++;
				}
			}
			else
			{
				count1=ydiff1;          //calculate y increment for edge1
				count2=ydiff2;          //calculate y increment for edge2
				while(count1 && count2)
				{
					errorterm1+=xdiff1;
					if(errorterm1>=ydiff1)
					{
						errorterm1-=ydiff1;
						offset1+=xunit1;
						xstart1+=xunit1;
					}
					count1--;
					// calculate edge 2
					errorterm2+=xdiff2;
					if(errorterm2>=ydiff2)
					{
						errorterm2-=ydiff2;
						offset2+=xunit2;
						xstart2+=xunit2;
					}
					--count2;
					length=offset2-offset1;
					if(length<0)
					{
						length=-length;
						start=offset2;
					}
					else
						start=offset1;
					for(int i=start; i<start+length+1; i++)
						vbuffer16[i]=(USHORT)m_Color;
					offset1+=lpitch;
					ystart1++;
					offset2+=lpitch;
					ystart2++;
				}//end while
			}//end else
		}//end else
		if(!count1)
		{
			--edgecount;
			startvert1=endvert1;
			--endvert1;

			if(endvert1<0)
				endvert1=m_NumVerts-1;
			xend1=(int)m_pVertex[endvert1].x+m_CenterPoint.x;
			yend1=(int)m_pVertex[endvert1].y+m_CenterPoint.y;
		}
		if(!count2)
		{
			--edgecount;
			startvert2=endvert2;
			endvert2++;

			if(endvert2==m_NumVerts)
				endvert2=0;
			xend2=(int)m_pVertex[endvert2].x+m_CenterPoint.x;
			yend2=(int)m_pVertex[endvert2].y+m_CenterPoint.y;
		}
	}
	return true;
}

