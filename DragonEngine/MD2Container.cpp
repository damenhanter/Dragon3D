#include "StdAfx.h"
#include "MD2Container.h"


MD2ANIM md2Anim[MD2_NUM_ANIMATIONS]  = 
{
	{0,39,0.5,1},    // MD2_ANIM_STATE_STANDING_IDLE       0
	{40,45,0.5,2},   // MD2_ANIM_STATE_RUN                 1
	{46,53,0.5,1},   // MD2_ANIM_STATE_ATTACK              2 
	{54,57,0.5,1},   // MD2_ANIM_STATE_PAIN_1              3
	{58,61,0.5,1},   // MD2_ANIM_STATE_PAIN_2              4
	{62,65,0.5,1},   // MD2_ANIM_STATE_PAIN_3              5
	{66,71,0.5,1},   // MD2_ANIM_STATE_JUMP                6
	{72,83,0.5,1},   // MD2_ANIM_STATE_FLIP                7
	{84,94,0.5,1},   // MD2_ANIM_STATE_SALUTE              8
	{95,111,0.5,1},  // MD2_ANIM_STATE_TAUNT               9
	{112,122,0.5,1}, // MD2_ANIM_STATE_WAVE                10
	{123,134,0.5,1}, // MD2_ANIM_STATE_POINT               11 
	{135,153,0.5,1}, // MD2_ANIM_STATE_CROUCH_STAND        12 
	{154,159,0.5,1}, // MD2_ANIM_STATE_CROUCH_WALK         13
	{160,168,0.5,1}, // MD2_ANIM_STATE_CROUCH_ATTACK       14
	{169,172,0.5,1}, // MD2_ANIM_STATE_CROUCH_PAIN         15
	{173,177,0.25,0}, // MD2_ANIM_STATE_CROUCH_DEATH        16  
	{178,183,0.25,0}, // MD2_ANIM_STATE_DEATH_BACK          17
	{184,189,0.25,0}, // MD2_ANIM_STATE_DEATH_FORWARD       18
	{190,197,0.25,0} // MD2_ANIM_STATE_DEATH_SLOW          19
};

char *md2AnimStrings[] = 
{
	"MD2_ANIM_STATE_STANDING_IDLE ",
	"MD2_ANIM_STATE_RUN",
	"MD2_ANIM_STATE_ATTACK", 
	"MD2_ANIM_STATE_PAIN_1",
	"MD2_ANIM_STATE_PAIN_2",
	"MD2_ANIM_STATE_PAIN_3",
	"MD2_ANIM_STATE_JUMP",
	"MD2_ANIM_STATE_FLIP",
	"MD2_ANIM_STATE_SALUTE",
	"MD2_ANIM_STATE_TAUNT",
	"MD2_ANIM_STATE_WAVE",
	"MD2_ANIM_STATE_POINT",       
	"MD2_ANIM_STATE_CROUCH_STAND", 
	"MD2_ANIM_STATE_CROUCH_WALK ",
	"MD2_ANIM_STATE_CROUCH_ATTACK",
	"MD2_ANIM_STATE_CROUCH_PAIN",
	"MD2_ANIM_STATE_CROUCH_DEATH",  
	"MD2_ANIM_STATE_DEATH_BACK",
	"MD2_ANIM_STATE_DEATH_FORWARD",
	"MD2_ANIM_STATE_DEATH_SLOW",
};


CMD2Container::CMD2Container(void)
{
	plist=NULL;
	textlist=NULL;
	vlist=NULL;
	skin=NULL;
}

CMD2Container::~CMD2Container(void)
{
	if (plist)
	{
		free(plist);
		plist=NULL;
	}
	if (textlist)
	{
		free(textlist);
		textlist=NULL;
	}
	if (vlist)
	{
		free(vlist);
		vlist=NULL;
	}
	
}

BOOL CMD2Container::LoadMD2Object(char* modelFile,CVector4& scale,CVector4& pos,CVector4& rot,
								  char* textureFile,int attr,int color,int VertexFlag)
{
	FILE* fp=NULL;    //指向模型文件指针
	int flength=-1;   //存储文件长度
	UCHAR* buffer=NULL;   //用于存储md2文件中的数据

	LPMD2HEADER md2Header;  //指向文件头

	if ((fp=fopen(modelFile,"rb"))==NULL)
	{
		MessageBox(NULL,"md2文件打开失败！","",MB_OK);
		return FALSE;
	}
	
	//移至文件尾
	fseek(fp,0,SEEK_END);
	//确定文件长度
	flength=ftell(fp);

	//将文件读入缓冲区

	//将文件指针重置文件开头
	fseek(fp,0,SEEK_SET);
	buffer=(UCHAR*)malloc(flength+1);

	int iret=fread(buffer,sizeof(UCHAR),flength,fp);

	//缓冲区得开头为文件头，因此用文件头指针指向它
	md2Header=(LPMD2HEADER)buffer;

	//检查文件类型
	if (md2Header->identifier!=MD2_MAGIC_NUM||
		md2Header->version!=MD2_VERSION)
	{
		fclose(fp);
		return FALSE;
	}

	//设置容器字段
	state=0;
	this->attr=attr;
	this->color=color;
	numFrames=md2Header->numFrames;
	numTriangles=md2Header->numTris;
	numVerts=md2Header->numVerts;
	numTextcoords=md2Header->numTextcoords;
	currFrame=0;
	skin=NULL;
	worldPos=pos;

	//分配用于存储网格数据的内存
	plist=(LPMD2TRI)malloc(md2Header->numTris*sizeof(MD2TRI));

	vlist=(CVector4*)malloc(md2Header->numFrames*md2Header->numVerts*sizeof(CVector4));

	textlist=(CVector2*)malloc(md2Header->numTextcoords*sizeof(CVector2));

	for (int i=0;i<md2Header->numTextcoords;i++)
	{
		textlist[i].x=((LPMD2TEXTCOORD)(buffer+md2Header->offsetTextcoords))[i].u;
		textlist[i].y=((LPMD2TEXTCOORD)(buffer+md2Header->offsetTextcoords))[i].v;
	}

	for (int frame=0;frame<md2Header->numFrames;frame++)
	{
		LPMD2FRAME ptrFrame=(LPMD2FRAME)(buffer+md2Header->offsetFrames+md2Header->frameSize*frame);
		
		float sx=ptrFrame->scale[0];
		float sy=ptrFrame->scale[1];
		float sz=ptrFrame->scale[2];
		float tx=ptrFrame->translate[0];
		float ty=ptrFrame->translate[1];
		float tz=ptrFrame->translate[2];

		for (int vert=0;vert<md2Header->numVerts;vert++)
		{
			CVector4 v;
			v.x=ptrFrame->vlist[vert].v[0]*sx+tx;
			v.y=ptrFrame->vlist[vert].v[1]*sy+ty;
			v.z=ptrFrame->vlist[vert].v[2]*sz+tz;
			//根据传入数据进行缩放
			v.x=v.x*scale.x;
			v.y=v.y*scale.y;
			v.z=v.z*scale.z;

			if (VertexFlag & VERTEX_FLAGS_INVERT_X)
			{
				v.x=-v.x;
			}
			if (VertexFlag & VERTEX_FLAGS_INVERT_Y)
			{
				v.y=-v.y;
			}
			if (VertexFlag & VERTEX_FLAGS_INVERT_Z)
			{
				v.z=-v.z;
			}
			if (VertexFlag & VERTEX_FLAGS_SWAP_YZ)
			{
				swap(v.y,v.z);
			}
			if (VertexFlag & VERTEX_FLAGS_SWAP_XZ)
			{
				swap(v.x,v.z);
			}
			if (VertexFlag & VERTEX_FLAGS_SWAP_XY)
			{
				swap(v.x,v.y);
			}
			vlist[vert+frame*md2Header->numVerts]=v;
		}
	}

	LPMD2TRI ptrTriangle=(LPMD2TRI)(buffer+md2Header->offsetTris);

	for (int tri=0;tri<md2Header->numTris;tri++)
	{
		if (VertexFlag & VERTEX_FLAGS_INVERT_WINDING_ORDER)
		{
			//反转环绕顺序
			plist[tri].vindex[0]=ptrTriangle[tri].vindex[2];
			plist[tri].vindex[1]=ptrTriangle[tri].vindex[1];
			plist[tri].vindex[2]=ptrTriangle[tri].vindex[0];

			plist[tri].tindex[0]=ptrTriangle[tri].tindex[2];
			plist[tri].tindex[1]=ptrTriangle[tri].tindex[1];
			plist[tri].tindex[2]=ptrTriangle[tri].tindex[0];
		}
		else
		{
			plist[tri].vindex[0]=ptrTriangle[tri].vindex[0];
			plist[tri].vindex[1]=ptrTriangle[tri].vindex[1];
			plist[tri].vindex[2]=ptrTriangle[tri].vindex[2];

			plist[tri].tindex[0]=ptrTriangle[tri].tindex[0];
			plist[tri].tindex[1]=ptrTriangle[tri].tindex[1];
			plist[tri].tindex[2]=ptrTriangle[tri].tindex[2];
		}
	}

	fclose(fp);

	CBitmapFile bitFile;
	bitFile.LoadBitmapFromFile(textureFile);
	skin=new CBitmapImage;
	skin->CreateBitmapImage(0,0,bitFile.bitmapinfoheader.biWidth,
		bitFile.bitmapinfoheader.biHeight,bitFile.bitmapinfoheader.biBitCount);
	skin->LoadImageBitmap16(bitFile,0,0,BITMAP_EXTRACT_MODE_ABS);
	bitFile.UnLoadBitmapFile();

	if (buffer)
	{
		free(buffer);
		buffer=NULL;
	}
	return TRUE;
}

void CMD2Container::SetAnimation(int animState,int animMode)
{
	this->animState=animState;
	this->animCounter=0;
	this->animSpeed=md2Anim[animState].animSpeed;

	this->currFrame=md2Anim[animState].startFrame;

	this->animComplete=0;
}

void CMD2Container::DoAnimation()
{
	if (++animCounter>=animSpeed)
	{
		animCounter=0;
		currFrame+=md2Anim[animState].irate;
		if (currFrame>md2Anim[animState].endFrame)
		{
			if (animMode==MD2_ANIM_LOOP)
			{
				currFrame=md2Anim[animState].startFrame;
			}
			else
			{
				currFrame=md2Anim[animState].endFrame;
				animComplete=1;
			}
		}
	}
}
