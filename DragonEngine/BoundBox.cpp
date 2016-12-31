#include "StdAfx.h"
#include "BoundBox.h"

CBoundBox::CBoundBox(void)
:xmin(0),xmax(0),ymin(0),ymax(0)
{
}

CBoundBox::~CBoundBox(void)
{
}

int CBoundBox::CreateColliCheck(CPolygon2D* polygon2D)
{
	xmin=(int)polygon2D->m_pVertex[0].x;
	xmax=(int)polygon2D->m_pVertex[0].x;
	ymin=(int)polygon2D->m_pVertex[0].y;
	ymax=(int)polygon2D->m_pVertex[0].y;
	for(int index=1; index<polygon2D->m_NumVerts; index++)
	{
		if(polygon2D->m_pVertex[index].x<xmin)
			xmin=(int)polygon2D->m_pVertex[index].x;
		if(polygon2D->m_pVertex[index].x>xmax)
			xmax=(int)polygon2D->m_pVertex[index].x;
		if(polygon2D->m_pVertex[index].y<ymin)
			ymin=(int)polygon2D->m_pVertex[index].y;
		if(polygon2D->m_pVertex[index].y>ymax)
			ymax=(int)polygon2D->m_pVertex[index].y;
	}
	return true;
}

int CBoundBox::IsPointInside(int x, int y)
{
	if(x>xmax || x<xmin || y<ymin || y>ymax)
		return false;
	return true;
}

BOOL CBoundBox::IsCollision(CBoundBox* Bounding)
{
	if(xmin>Bounding->xmax || xmax<Bounding->xmin)
		return false;
	if(ymin>Bounding->ymax || ymax<Bounding->xmin)
		return false;
	return true;
}
