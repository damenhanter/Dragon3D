#pragma  once
#include "stdafx.h"
#include "BSPEditor.h"

extern HWND hWnd;
CLine2D drawline;

GADGET Buttons[NUM_GADGET] = 
{ 
	{GADGET_TYPE_TOGGLE,    GADGET_ON,  GADGET_ID_SEGMENTMODE,    670, 50,          116,32, NULL,0}, 
	{GADGET_TYPE_TOGGLE,    GADGET_OFF, GADGET_ID_POLYLINEMODE,  670, 50+(50-12),  116,32, NULL,0}, 
	{GADGET_TYPE_TOGGLE,    GADGET_OFF, GADGET_ID_DELETEMODE,    670, 50+(100-24), 116,32, NULL,0}, 
	{GADGET_TYPE_MOMENTARY, GADGET_OFF, GADGET_ID_CLEARALL,      670, 348,         116,32, NULL,0}, 
	{GADGET_TYPE_TOGGLE,    GADGET_OFF, GADGET_ID_FLOORMODE,     670, 556,         116,32, NULL,0}, 

	{GADGET_TYPE_STATIC,    GADGET_OFF, GADGET_ID_WALL_TEXTURE_DEC,  680, 316,         16,16, NULL,0}, 
	{GADGET_TYPE_STATIC,    GADGET_OFF, GADGET_ID_WALL_TEXTURE_INC,  700, 316,         16,16, NULL,0}, 

	{GADGET_TYPE_STATIC,    GADGET_OFF, GADGET_ID_WALL_HEIGHT_DEC,   680, 208,         16,16, NULL,0}, 
	{GADGET_TYPE_STATIC,    GADGET_OFF, GADGET_ID_WALL_HEIGHT_INC,   700, 208,         16,16, NULL,0}, 

	{GADGET_TYPE_STATIC,    GADGET_OFF, GADGET_ID_FLOOR_TEXTURE_DEC, 680, 522,         16,16, NULL,0}, 
	{GADGET_TYPE_STATIC,    GADGET_OFF, GADGET_ID_FLOOR_TEXTURE_INC, 700, 522,         16,16, NULL,0}, 
};

CEditor::CEditor(CRenderSystem* render)
{
	this->render=render;
	editorState=EDITOR_STATE_INIT;
	action=ACTION_STARTING_LINE;
	wallList=root=NULL;
}

CEditor::~CEditor()
{
	if (ptrModel)
	{
		delete ptrModel;
		ptrModel=NULL;
	}
}
void CEditor::ResetEditor()
{
	
	totalLines=0;
	startingLine=0;
	snapGrid=1;
	viewGrid=1;
	viewWalls=1;
	viewFloors=1;
	nearestLine=-1;
	textureIndexWall=11;
	textureIndexFloor=13;
	wallHeight=64;
	ceilingHeight=0;
	numFloorTiles=0;
	ptrModel=NULL;
	polyAttr=(TRI4DV1_ATTR_DISABLE_MATERIAL
		| TRI4DV1_ATTR_2SIDED | TRI4DV1_ATTR_RGB16 | TRI4DV1_ATTR_SHADE_MODE_FLAT );
	polyColor=_RGB16BIT565(255,255,255);

	for (int celly=0;celly<BSP_CELLS_Y-1;celly++)
	{
		for (int cellx=0;cellx<BSP_CELLS_X-1;cellx++)
		{
			floors[celly][cellx]=-1;
		}
	}
	memset(lines,0,sizeof(BSPLINE)*MAX_LINES);
}

void CEditor::DrawFloors()
{
	numFloorTiles=0;
	for (int celly=0;celly<BSP_CELLS_Y-1;celly++)
	{
		for (int cellx=0;cellx<BSP_CELLS_X-1;cellx++)
		{
			int textid=floors[celly][cellx];
			//纹理存在吗？
			if (textid>-1)
			{
				//在编辑器空间设置纹理坐标
				textures.x=BSP_MIN_X+(cellx*BSP_GRID_SIZE);
				textures.y=BSP_MIN_Y+(celly*BSP_GRID_SIZE);
				textures.currFrame=textid;
				textures.RenderScaleAnim(render,BSP_GRID_SIZE,BSP_GRID_SIZE,render->lpddsback);
				numFloorTiles++;
			}
		}
	}
}

void CEditor::DrawLines(DIMMOUSE& mouseState)
{
	for (int index=0;index<totalLines;index++)
	{
		if (lines[index].textureid>=0)
		{
			drawline.DrawClipLine162D(lines[index].p0.x,lines[index].p0.y,
				lines[index].p1.x,lines[index].p1.y,_RGB16BIT565(255,0,0),
				render->backBuffer,render->backlpitch);
		}
		else
		{
			drawline.DrawClipLine162D(lines[index].p0.x,lines[index].p0.y,
				lines[index].p1.x,lines[index].p1.y,_RGB16BIT565(128,128,128),
				render->backBuffer,render->backlpitch);
		}
		// mark endpoints with X
		drawline.DrawClipLine162D( lines[index].p0.x-4, lines[index].p0.y-4,
			lines[index].p0.x+4, lines[index].p0.y+4,
			_RGB16BIT565(0,255,0),render->backBuffer,render->backlpitch);

		drawline.DrawClipLine162D( lines[index].p0.x+4, lines[index].p0.y-4,
			lines[index].p0.x-4, lines[index].p0.y+4,
			_RGB16BIT565(0,255,0), render->backBuffer,render->backlpitch);

		drawline.DrawClipLine162D( lines[index].p1.x-4, lines[index].p1.y-4,
			lines[index].p1.x+4, lines[index].p1.y+4,
			_RGB16BIT565(0,255,0),render->backBuffer,render->backlpitch);

		drawline.DrawClipLine162D( lines[index].p1.x+4, lines[index].p1.y-4,
			lines[index].p1.x-4, lines[index].p1.y+4,
			_RGB16BIT565(0,255,0),render->backBuffer,render->backlpitch);
	}
	// now draw special lines
	POINT p0;
	p0.x=mouseState.x;
	p0.y=mouseState.y;

	if ((action==ACTION_ENDING_LINE) &&
		(p0.x>BSP_MIN_X)&&(p0.x<BSP_MAX_X)
		&&(p0.y>BSP_MIN_Y)&&(p0.x<BSP_MAX_Y))
	{
		drawline.DrawClipLine162D( lines[totalLines].p0.x, lines[totalLines].p0.y,
			p0.x,p0.y,_RGB16BIT565(255,255,255),render->backBuffer,render->backlpitch);
	}

	if ((action==ACTION_ADDING_POLYLINE) &&
		(p0.x>BSP_MIN_X)&&(p0.x<BSP_MAX_X)
		&&(p0.y>BSP_MIN_Y)&&(p0.x<BSP_MAX_Y))
	{
		drawline.DrawClipLine162D( lines[totalLines].p0.x, lines[totalLines].p0.y,
			p0.x,p0.y,_RGB16BIT565(255,255,255),render->backBuffer,render->backlpitch);
	}

	static int redCounter=0;
	if ((redCounter+=32)>255)
	{
		redCounter=0;
	}

	if (p0.x > BSP_MIN_X && p0.x < BSP_MAX_X && 
		p0.y > BSP_MIN_Y && p0.y < BSP_MAX_Y)
	{
		if (nearestLine >= 0)
		{
			if (action == ACTION_DELETE_LINE)
			{
				drawline.DrawClipLine162D( lines[nearestLine].p0.x, lines[nearestLine].p0.y,
					lines[nearestLine].p1.x, lines[nearestLine].p1.y,
					_RGB16BIT565(redCounter,redCounter,redCounter), render->backBuffer, render->backlpitch);
			} 
			else
			{
				drawline.DrawClipLine162D( lines[nearestLine].p0.x, lines[nearestLine].p0.y,
					lines[nearestLine].p1.x, lines[nearestLine].p1.y,
					_RGB16BIT565(255,255,255), render->backBuffer, render->backlpitch);
			} 
		} 
	}        
}

int CEditor::DeleteLines(int x,int y,BOOL deleteline)
{
	int currLine;
	int bestLine=-1;
	float sx,sy;                    // starting coordinates of test line
	float ex,ey;                    // ending coordinates of test line
	float lengthLine;               // total length of line being tested
	float length1,length2;          // length of lines from endpoints of test line to target area
	float min_x,max_x,min_y,max_y;  // bounding box of test line
	float bestError=10000;         // start error off really large
	float testError;               // current error being processed

	// process each line and find best fit
	for (currLine=0; currLine < totalLines; currLine++)
	{
		// extract line parameters
		sx = lines[currLine].p0.x;
		sy = lines[currLine].p0.y;

		ex = lines[currLine].p1.x;
		ey = lines[currLine].p1.y;

		// first compute length of line
		lengthLine = Distance2D((ex-sx),(ey-sy));
		
		// compute length of first endpoint to selected position
		length1    = Distance2D( (x-sx) , (y-sy));

		// compute length of second endpoint to selected position
		length2    = Distance2D( (ex-x) , (ey-y));

		// compute the bounding box of line
		min_x = min(sx,ex)-2;
		min_y = min(sy,ey)-2;
		max_x = max(sx,ex)+2;
		max_y = max(sy,ey)+2;

		// if the selection position is within bounding box then compute distance
		// errors and save this line as a possibility

		if (x >= min_x && x <= max_x && y >= min_y && y <= max_y)
		{
			// compute percent error of total length to length of lines
			// from endpoint to selected position
			testError = (float)(100 * fabs( (length1 + length2) - lengthLine) ) / (float)lengthLine;

			// test if this line is a better selection than the last
			if (testError < bestError)
			{
				bestError = testError;
				bestLine  = currLine;
			}
		} 
	} 

	// did we get a line to delete?
	if (bestLine != -1)
	{
		// delete the line from the line array by copying another line into
		// the position
		if (deleteline)
		{
			if (totalLines == 1) // a single line
			{
				totalLines = 0;
			}
			else
			{
				if (bestLine == (totalLines-1))  // the line to delete is the last in array
				{
					totalLines--;
				}
				else
				{
					// the line to delete must be in the 0th to total_lines-1 position
					// so copy the last line into the deleted one and decrement the
					// number of lines in system
					lines[bestLine] = lines[--totalLines];
				} 
			}
		} 
		// return the line number that was deleted
		return bestLine;
	} 
	else
	{
		return -1;
	}
}

void CEditor::LoadTextureData()
{
	char buffer[256];
	buttonImages.CreateAnimation(render,0,0,128,48,10,
		ANIM_ATTR_VISIBLE | ANIM_ATTR_SINGLE_FRAME, DDSCAPS_SYSTEMMEMORY,0,16);
	buttonImages.attr|=ANIM_ATTR_VISIBLE;
	CBitmapFile bmpFile;
	bmpFile.LoadBitmapFromFile("..//res//bspguicon01.bmp");
	for (int i=0;i<10;i++)
	{
		buttonImages.LoadAnimFrame16(&bmpFile,i,i%2,i/2,BITMAP_EXTRACT_MODE_CELL);
	}
	
	Buttons[GADGET_ID_SEGMENTMODE].bmpImage=&buttonImages;
	Buttons[GADGET_ID_POLYLINEMODE].bmpImage=&buttonImages;
	Buttons[GADGET_ID_DELETEMODE].bmpImage=&buttonImages;
	Buttons[GADGET_ID_CLEARALL].bmpImage=&buttonImages;
	Buttons[GADGET_ID_FLOORMODE].bmpImage=&buttonImages;
	bmpFile.UnLoadBitmapFile();

	textures.CreateAnimation(render,0,0,TEXTURE_SIZE,TEXTURE_SIZE,
		NUM_TEXTURES,ANIM_ATTR_VISIBLE|ANIM_ATTR_SINGLE_FRAME,DDSCAPS_SYSTEMMEMORY,0,16);
	for (int i=0;i<NUM_TEXTURES;i++)
	{
		sprintf_s(buffer,"..//res//bspdemotext128_%d.bmp",i);
		bmpFile.LoadBitmapFromFile(buffer);
		textures.LoadAnimFrame16(&bmpFile,i,0,0,BITMAP_EXTRACT_MODE_ABS);

		textureMaps[i].CreateBitmapImage(0,0,TEXTURE_SIZE,TEXTURE_SIZE,16);
		textureMaps[i].LoadImageBitmap16(bmpFile,0,0,BITMAP_EXTRACT_MODE_ABS);

		bmpFile.UnLoadBitmapFile();
	}
}

void CEditor::DrawGadgets()
{
	for (int i=0;i<NUM_GADGET;i++)
	{
		LPGADGET currButton=&Buttons[i];
		switch (currButton->type)
		{
		case GADGET_TYPE_TOGGLE:
			{
				buttonImages.currFrame=currButton->id+currButton->state;

				buttonImages.x=currButton->x0;
				buttonImages.y=currButton->y0;
				
				buttonImages.RenderAnimation(render,render->lpddsback);
			}break;
		case GADGET_TYPE_MOMENTARY:
			{
				buttonImages.currFrame=currButton->id+currButton->state;

				buttonImages.x=currButton->x0;
				buttonImages.y=currButton->y0;

				buttonImages.RenderAnimation(render,render->lpddsback);
				// increment counter
				if (currButton->state == GADGET_ON)
				{
					if (++currButton->count > 2)
					{
						currButton->state = GADGET_OFF;
						currButton->count = 0;
					}
				}
			}break;
		case GADGET_TYPE_STATIC:
			{

			}break;
		default:
			break;
		}
	}
} 

int CEditor::ProcessGadgets(int mx,int my,DIMMOUSE& mouseState)
{
	for (int i=0;i<NUM_GADGET;i++)
	{
		LPGADGET currButton=&Buttons[i];
		if (mouseState.rcButtons[DIM_LB]| mouseState.rcButtons[DIM_MB] | mouseState.rcButtons[DIM_RB])
		{
			if ( (mx > currButton->x0) && (mx < currButton->x0+currButton->width) && 
				(my > currButton->y0) && (my < currButton->y0+currButton->height) )
			{
				switch (currButton->type)
				{
				case GADGET_TYPE_TOGGLE:
					{
						if (currButton->state == GADGET_OFF)
						{
							currButton->state = GADGET_ON;
						}
						for (int j = 0; j < NUM_GADGET; j++)
						{
							if (j!=i && Buttons[j].type == GADGET_TYPE_TOGGLE)
							{
								Buttons[j].state = GADGET_OFF;
							}
						}
						return currButton->id;
					}break;
				case GADGET_TYPE_MOMENTARY: // lights up while you press it
					{
						currButton->state = GADGET_ON;
						return currButton->id;

					} break;

				case GADGET_TYPE_STATIC:    
					{
						return currButton->id;
					} break;

				default:break;
					
				}
			}
		}
	}
	return -1;
}

void CEditor::DrawGrid(int rgbcolor, int x0, int y0, int xpitch, int ypitch,int xcells, int ycells)
{
	for (int xc=0; xc < xcells; xc++)
	{
		for (int yc=0; yc < ycells; yc++)
		{
			render->DrawPixel(x0 + xc*xpitch, y0 + yc*ypitch, rgbcolor);
		}
	}
}

void CEditor::ConvertLinesToWalls()
{
	LPBSPNODE last; // used to track the last wall processed
	LPBSPNODE temp;

	CVector4 u,v;

	// process each 2-d line and convert it into a 3-d wall
	for (int index=0;index<totalLines;index++)
	{
		temp=new BSPNODE;
		memset(temp,0,sizeof(BSPNODE));

		temp->front=temp->back=temp->link=NULL;

		temp->wall.vlist[0].x=WORLD_SCALE_X*(SCREEN_TO_WORLD_X+lines[index].p0.x);
		temp->wall.vlist[0].y=lines[index].elev+lines[index].height;
		temp->wall.vlist[0].z=WORLD_SCALE_Z*(SCREEN_TO_WORLD_Z+lines[index].p0.y);
		temp->wall.vlist[0].w=1;

		temp->wall.vlist[1].x=WORLD_SCALE_X*(SCREEN_TO_WORLD_X+lines[index].p1.x);
		temp->wall.vlist[1].y=lines[index].elev+lines[index].height;
		temp->wall.vlist[1].z=WORLD_SCALE_Z*(SCREEN_TO_WORLD_Z+lines[index].p1.y);
		temp->wall.vlist[1].w=1;

		temp->wall.vlist[2].x=WORLD_SCALE_X*(SCREEN_TO_WORLD_X+lines[index].p1.x);
		temp->wall.vlist[2].y=lines[index].elev;
		temp->wall.vlist[2].z=WORLD_SCALE_Z*(SCREEN_TO_WORLD_Z+lines[index].p1.y);
		temp->wall.vlist[2].w=1;

		temp->wall.vlist[3].x=WORLD_SCALE_X*(SCREEN_TO_WORLD_X+lines[index].p0.x);
		temp->wall.vlist[3].y=lines[index].elev;
		temp->wall.vlist[3].z=WORLD_SCALE_Z*(SCREEN_TO_WORLD_Z+lines[index].p0.y);
		temp->wall.vlist[3].w=1;

		// compute normal to wall
		u.CreateVector(temp->wall.vlist[0].v,temp->wall.vlist[1].v);
		v.CreateVector(temp->wall.vlist[0].v,temp->wall.vlist[3].v);

		temp->wall.n=u.CrossProduct(v);
		temp->wall.nlength=temp->wall.n.GetLength();
		float invLength=1/temp->wall.nlength;
		temp->wall.n.x*=invLength;
		temp->wall.n.y*=invLength;
		temp->wall.n.z*=invLength;

		temp->id=index;
		temp->wall.attr=lines[index].attr;
		temp->wall.color=lines[index].color;

		SET_BIT(temp->wall.vlist[0].attr, VERTEX_ATTR_POINT); 
		SET_BIT(temp->wall.vlist[1].attr, VERTEX_ATTR_POINT);
		SET_BIT(temp->wall.vlist[2].attr, VERTEX_ATTR_POINT);
		SET_BIT(temp->wall.vlist[3].attr, VERTEX_ATTR_POINT);

		if ( (temp->wall.attr & TRI4DV1_ATTR_SHADE_MODE_GOURAUD) ||  
			(temp->wall.attr & TRI4DV1_ATTR_SHADE_MODE_FASTPHONG) )
		{
			SET_BIT(temp->wall.vlist[0].attr, VERTEX_ATTR_NORMAL); 
			SET_BIT(temp->wall.vlist[1].attr, VERTEX_ATTR_NORMAL);
			SET_BIT(temp->wall.vlist[2].attr, VERTEX_ATTR_NORMAL);
			SET_BIT(temp->wall.vlist[3].attr, VERTEX_ATTR_NORMAL);
		}
		
		if (lines[index].textureid>=0)
		{
			SET_BIT(temp->wall.attr,TRI4DV1_ATTR_SHADE_MODE_TEXTURE);
			temp->wall.ptrTexture=&textureMaps[lines[index].textureid];

			temp->wall.vlist[0].u0 = 0;
			temp->wall.vlist[0].v0 = 0;

			temp->wall.vlist[1].u0 = TEXTURE_SIZE-1;
			temp->wall.vlist[1].v0 = 0;

			temp->wall.vlist[2].u0 = TEXTURE_SIZE-1;
			temp->wall.vlist[2].v0 = TEXTURE_SIZE-1;

			temp->wall.vlist[3].u0 = 0;
			temp->wall.vlist[3].v0 = TEXTURE_SIZE-1;

			SET_BIT(temp->wall.vlist[0].attr, VERTEX_ATTR_TEXTURE); 
			SET_BIT(temp->wall.vlist[1].attr, VERTEX_ATTR_TEXTURE); 
			SET_BIT(temp->wall.vlist[2].attr, VERTEX_ATTR_TEXTURE); 
			SET_BIT(temp->wall.vlist[3].attr, VERTEX_ATTR_TEXTURE); 
		}

		temp->wall.state=TRI4DV1_STATE_ACTIVE;
		if ((lines[index].elev + lines[index].height) > ceilingHeight)
		{
			ceilingHeight = lines[index].elev + lines[index].height;
		}
		
		if (index==0)
		{
			wallList=temp;
			last=temp;
		}
		else
		{
			last->link=temp;
			last=temp;
		}
	}
}

int CEditor::GenerateFloors(int color,CPoint4D& vpos,CPoint4D& vrot,int TriAttr)
{
	CBitmapFile bmpFile;

	ptrModel=new CCOBModel;
	memset(ptrModel,0,sizeof(CCOBModel));

	ptrModel->state=OBJMODEL4DV1_STATE_ACTIVE|OBJMODEL4DV1_STATE_VISIBLE;
	ptrModel->worldPos=vpos;

	ptrModel->attr=OBJMODEL4DV1_ATTR_SINGLE_FRAME;

	int floorTiles=0;
	ptrModel->numVertices=0;
	ptrModel->nTriangles=0;
	ptrModel->currFrame=0;
	ptrModel->numFrames=1;

	for (int y=0;y<BSP_CELLS_Y-1;y++)
	{
		for (int x=0;x<BSP_CELLS_X-1;x++)
		{
			if (floors[y][x]>=0)
			{
				ptrModel->nTriangles+=2;
				ptrModel->numVertices+=4;
			}
		}
	}
	ptrModel->InitModel(ptrModel->numVertices,ptrModel->nTriangles,ptrModel->numFrames);
	SET_BIT(ptrModel->attr,OBJMODEL4DV1_ATTR_TEXTURES);
	int currVertex=0,currTri=0;

	for (int y=0;y<BSP_CELLS_Y-1;y++)
	{
		for (int x=0;x<BSP_CELLS_X-1;x++)
		{
			int textureid=-1;
			if ((textureid=floors[y][x])>=0)
			{
				//Vertex 0
				ptrModel->vlocalList[currVertex].x=WORLD_SCALE_X * (SCREEN_TO_WORLD_X + (x+0)*BSP_GRID_SIZE);
				ptrModel->vlocalList[currVertex].y=0;
				ptrModel->vlocalList[currVertex].z=WORLD_SCALE_Z * (SCREEN_TO_WORLD_Z + (y+1)*BSP_GRID_SIZE);
				ptrModel->vlocalList[currVertex].w=1;
				
				ptrModel->textlist[currVertex].x=0;
				ptrModel->textlist[currVertex].y=TEXTURE_SIZE-1;

				SET_BIT(ptrModel->vlocalList[currVertex].attr,VERTEX_ATTR_POINT);
				currVertex++;

				//Vertex 1
				ptrModel->vlocalList[currVertex].x=WORLD_SCALE_X * (SCREEN_TO_WORLD_X + (x+0)*BSP_GRID_SIZE);
				ptrModel->vlocalList[currVertex].y=0;
				ptrModel->vlocalList[currVertex].z=WORLD_SCALE_Z * (SCREEN_TO_WORLD_Z + (y+0)*BSP_GRID_SIZE);
				ptrModel->vlocalList[currVertex].w=1;

				ptrModel->textlist[currVertex].x=0;
				ptrModel->textlist[currVertex].y=0;

				SET_BIT(ptrModel->vlocalList[currVertex].attr,VERTEX_ATTR_POINT);
				currVertex++;

				//Vertex 2
				ptrModel->vlocalList[currVertex].x=WORLD_SCALE_X * (SCREEN_TO_WORLD_X + (x+1)*BSP_GRID_SIZE);
				ptrModel->vlocalList[currVertex].y=0;
				ptrModel->vlocalList[currVertex].z=WORLD_SCALE_Z * (SCREEN_TO_WORLD_Z + (y+0)*BSP_GRID_SIZE);
				ptrModel->vlocalList[currVertex].w=1;

				ptrModel->textlist[currVertex].x=TEXTURE_SIZE-1;
				ptrModel->textlist[currVertex].y=0;

				SET_BIT(ptrModel->vlocalList[currVertex].attr,VERTEX_ATTR_POINT);
				currVertex++;

				//Vertex 3
				ptrModel->vlocalList[currVertex].x=WORLD_SCALE_X * (SCREEN_TO_WORLD_X + (x+1)*BSP_GRID_SIZE);
				ptrModel->vlocalList[currVertex].y=0;
				ptrModel->vlocalList[currVertex].z=WORLD_SCALE_Z * (SCREEN_TO_WORLD_Z + (y+1)*BSP_GRID_SIZE);
				ptrModel->vlocalList[currVertex].w=1;

				ptrModel->textlist[currVertex].x=TEXTURE_SIZE-1;
				ptrModel->textlist[currVertex].y=TEXTURE_SIZE-1;

				SET_BIT(ptrModel->vlocalList[currVertex].attr,VERTEX_ATTR_POINT);
				currVertex++;

				//左上角多边形定点索引
				ptrModel->plist[currTri].vert[0]=currVertex-4;//0
				ptrModel->plist[currTri].vert[1]=currVertex-3;//1
				ptrModel->plist[currTri].vert[2]=currVertex-2;//2
				//右下角多边形索引
				ptrModel->plist[currTri+1].vert[0]=currVertex-4;//0
				ptrModel->plist[currTri+1].vert[1]=currVertex-2;//2
				ptrModel->plist[currTri+1].vert[2]=currVertex-1;//3

				ptrModel->plist[currTri].vlist=ptrModel->vlocalList;
				ptrModel->plist[currTri+1].vlist=ptrModel->vlocalList;

				ptrModel->plist[currTri].attr=TriAttr;
				ptrModel->plist[currTri+1].attr=TriAttr;

				ptrModel->plist[currTri].color=color;
				ptrModel->plist[currTri+1].color=color;

				ptrModel->plist[currTri].ptrTexture=&textureMaps[floors[y][x]];
				ptrModel->plist[currTri+1].ptrTexture=&textureMaps[floors[y][x]];

				// upper left triangle
				ptrModel->plist[currTri].text[0] = currVertex-4; // 0
				ptrModel->plist[currTri].text[1] = currVertex-3; // 1
				ptrModel->plist[currTri].text[2] = currVertex-2; // 2

				// lower right triangle
				ptrModel->plist[currTri+1].text[0] = currVertex-4; // 0
				ptrModel->plist[currTri+1].text[1] = currVertex-2; // 2
				ptrModel->plist[currTri+1].text[2] = currVertex-1; // 3

				SET_BIT(ptrModel->vlocalList[ ptrModel->plist[currTri].vert[0] ].attr, VERTEX_ATTR_TEXTURE); 
				SET_BIT(ptrModel->vlocalList[ ptrModel->plist[currTri].vert[1] ].attr, VERTEX_ATTR_TEXTURE); 
				SET_BIT(ptrModel->vlocalList[ ptrModel->plist[currTri].vert[2] ].attr, VERTEX_ATTR_TEXTURE); 

				SET_BIT(ptrModel->vlocalList[ptrModel->plist[currTri+1].vert[0] ].attr, VERTEX_ATTR_TEXTURE); 
				SET_BIT(ptrModel->vlocalList[ ptrModel->plist[currTri+1].vert[1] ].attr, VERTEX_ATTR_TEXTURE); 
				SET_BIT(ptrModel->vlocalList[ ptrModel->plist[currTri+1].vert[2] ].attr, VERTEX_ATTR_TEXTURE); 

				SET_BIT(ptrModel->plist[currTri].attr, TRI4DV1_ATTR_DISABLE_MATERIAL);
				SET_BIT(ptrModel->plist[currTri+1].attr, TRI4DV1_ATTR_DISABLE_MATERIAL);

				ptrModel->plist[currTri].state   = TRI4DV1_STATE_ACTIVE;    
				ptrModel->plist[currTri+1].state = TRI4DV1_STATE_ACTIVE;  

				ptrModel->plist[currTri].vlist   =ptrModel->vlocalList; 
				ptrModel->plist[currTri+1].vlist =ptrModel->vlocalList; 

				ptrModel->plist[currTri].textlist   = ptrModel->textlist;
				ptrModel->plist[currTri+1].textlist = ptrModel->textlist;

				currTri+=2;
			}
		}
	}
	
	ptrModel->ComputeOBJTriLength();
	ptrModel->ComputeOBJVertexNormal();
	
	return TRUE;
}

int CEditor::LoadBSPFileLEV(char* filename)
{
	float version;     // holds the version number

	int numSections,  // number of sections
		numWalls,     // number of walls
		bsp_cells_x,   // number of cells on x,y axis
		bsp_cells_y;

	FILE *fp; 

	char token1[80], token2[80],buffer[256];

	if ((fp = fopen(filename, "r")) == NULL)
	{
		sprintf(buffer, "Error - File %s not found.",filename);
		MessageBox(hWnd, buffer, "BSP Level Generator",MB_OK);   
		return 0;
	}

	fscanf(fp, "%s %f", token1, &version);

	if (version!=1.0f)
	{
		sprintf(buffer, "Error %s - Wrong Version.",filename);
		MessageBox(hWnd, buffer, "BSP Level Generator",MB_OK);   
		return 0;
	}

	fscanf(fp, "%s %d", token1, &numSections);

	if (numSections!=2)
	{
		sprintf(buffer, "Error -%s Wrong Number of Sections.",filename);
		MessageBox(hWnd, buffer, "BSP Level Generator",MB_OK);   
		return 0;
	} 

	fscanf(fp,"%s %s", token1, token2);

	fscanf(fp,"%s %d", token1, &numWalls);

	for (int w_index = 0; w_index < numWalls; w_index++)
	{
		// x0.f y0.f x1.f y1.f elev.f height.f text_id.d color.id attr.d
		fscanf(fp, "%f %f %f %f %d %d %d %d %d", &lines[w_index].p0.x, 
			&lines[w_index].p0.y,
			&lines[w_index].p1.x, 
			&lines[w_index].p1.y,
			&lines[w_index].elev, 
			&lines[w_index].height,
			&lines[w_index].textureid,
			&lines[w_index].color,
			&lines[w_index].attr );
	} 

	fscanf(fp,"%s", token1);

	fscanf(fp,"%s %s", token1, token2);

	fscanf(fp,"%s %d", token1, &bsp_cells_x);

	fscanf(fp,"%s %d", token1, &bsp_cells_y);

	for (int y_index = 0; y_index < BSP_CELLS_Y-1; y_index++)
	{
		for (int x_index = 0; x_index < BSP_CELLS_X-1; x_index++)
		{
			fscanf(fp, "%d", &floors[y_index][x_index]);
		}       
	} 

	fscanf(fp,"%s", token1);

	totalLines = numWalls;
	// bsp_cells_x
	// bsp_cells_y

	fclose(fp);

	return(1);
}
int CEditor::SaveBSPFileLEV(char* filename)
{
	FILE *fp; 

	if ((fp = fopen(filename, "w")) == NULL)
		return 0;

	fprintf(fp, "\nVersion: 1.0");

	fprintf(fp, "\n");

	fprintf(fp, "\nNumSections: 2");

	fprintf(fp, "\n");

	fprintf(fp,"\nSection: walls");

	fprintf(fp, "\n");

	fprintf(fp,"\nNumWalls: %d", totalLines);

	fprintf(fp, "\n");

	for (int w_index = 0; w_index < totalLines; w_index++)
	{
		// x0.f y0.f x1.f y1.f elev.f height.f text_id.d
		fprintf(fp, "\n%d %d %d %d %d %d %d %d %d ", (int)lines[w_index].p0.x, 
			(int)lines[w_index].p0.y,
			(int)lines[w_index].p1.x, 
			(int)lines[w_index].p1.y,
			lines[w_index].elev, 
			lines[w_index].height,
			lines[w_index].textureid,
			lines[w_index].color,
			lines[w_index].attr);
	} 

	fprintf(fp, "\n");
 
	fprintf(fp,"\nEndSection");

	fprintf(fp, "\n");

	fprintf(fp,"\nSection: floors");

	fprintf(fp, "\n");

	fprintf(fp,"\nNumFloorsX: %d", BSP_CELLS_X-1);

	fprintf(fp,"\nNumFloorsY: %d", BSP_CELLS_Y-1);

	fprintf(fp, "\n");

	for (int y_index = 0; y_index < BSP_CELLS_Y-1; y_index++)
	{
		fprintf(fp, "\n");

		for (int x_index = 0; x_index < BSP_CELLS_X-1; x_index++)
		{
			fprintf(fp, "%d ", floors[y_index][x_index]);
		}    
	} 

	fprintf(fp, "\n");

	fprintf(fp,"\nEndSection");

	fclose(fp);

	return true;
}