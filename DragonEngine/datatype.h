#pragma once
#ifndef _DATATYPE_
#define _DATATYPE_

#define INITGUID
#define DIRECTINPUT_VERSION  0x0800

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>

#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <direct.h>
#include <wchar.h>
#include <math.h>
#include <SYS\TIMEB.H>
#include <time.h>
#include <io.h>
#include <fcntl.h>
#include <objbase.h>
#include <dinput.h>
#include <dsound.h>
#include <dmksctrl.h>
#include <dmusici.h>
#include <dmusicc.h>
#include <dmusicf.h>
#include <ddraw.h>

#pragma warning (disable:4996) 
#pragma warning (disable:4244) 

extern float cosTable[361];
extern float sinTable[361];
extern float invcosTable[362];

//ALPHA混合级别
#define NUM_ALPHA_LEVELS           8

extern USHORT AlphaTable[NUM_ALPHA_LEVELS][65535];

#define ClassName  "SCOTT" 

#define SCREEN_WIDTH       800
#define SCREEN_HEIGHT      600
#define SCREEN_BPP         16

#define BITMAP_ID           0x4D42         //位图统一id

#define PALETTE_DEFAULT  "..\\res\\PALDATA1.PAL"

#define DIM_LB             0
#define DIM_MB             1
#define DIM_RB             2

typedef struct MOUSE_TYP 
{
	int x;
	int y;
	BYTE rcButtons[3];
}DIMMOUSE,*LPDIMMOUSE;

enum BitmapState
{
	BITMAP_STATE_DEAD,
	BITMAP_STATE_ALIVE,
	BITMAP_STATE_DYING,
	BITMAP_EXTRACT_MODE_ABS,
	BITMAP_EXTRACT_MODE_CELL,
	BITMAP_ATTR_LOADED=128
};

enum DDPixelFormat
{
	DD_PIXEL_FORMAT8=8,
	DD_PIXEL_FORMAT555=15,
	DD_PIXEL_FORMAT565=16,
	DD_PIXEL_FORMAT888=24,
	DD_PIXEL_FORMATA888=32
};	

enum AnimState
{
	ANIM_STATE_DEAD,
	ANIM_STATE_ANIM_DONE,
	ANIM_STATE_ALIVE=1,
	ANIM_STATE_DYING,
	ANIM_MAX_ANIMATIONS=16
};

#define ANIM_MAX_FRAMES   64
#define ANIM_MAX_ANIMATIONS   32
//DRAGON 属性
enum AnimAddr
{
	ANIM_ATTR_SINGLE_FRAME=1,
	ANIM_ATTR_MULTI_FRAME=2,
	ANIM_ATTR_MULTI_ANIM=4,
	ANIM_ATTR_ANIM_ONE_SHOT=8,
	ANIM_ATTR_VISIBLE=16,
	ANIM_ATTR_BOUNCE=32,
	ANIM_ATTR_WRAPAROUND=64,
	ANIM_ATTR_LOADED=128,
	ANIM_ATTR_CLONE=256
};
enum AnimDirection
{
	ANIM_EAST,
	ANIM_NEAST,
	ANIM_NORTH,
	ANIM_NWEST,
	ANIM_WEST,
	ANIM_SWEST,
	ANIM_SOUTH,
	ANIM_SEAST
};
//屏幕移动
enum ScreenAttr
{
	SCREEN_DARKNESS,
	SCREEN_WHITENESS,
	SCREEN_SWIPE_X,
	SCREEN_SWIPE_Y,
	SCREEN_DISOLVE,
	SCREEN_SCRUNCH,
	SCREEN_BLUENESS,
	SCREEN_REDNESS,
	SCREEN_GREENNESS
};

enum Blinker
{
	BLINKER_ADD,
	BLINKER_DELETE,
	BLINKER_UPDATE,
	BLINKER_RUN
};

enum MIDIState
{
	MIDI_NULL,
	MIDI_LOADED,
	MIDI_PLAYING,
	MIDI_STOPPED
};

enum SoundState
{
	SOUND_NULL,
	SOUND_LOADED,
	SOUND_PLAYING,
	SOUND_STOPPED
};

#define PARTICLE_STATE_DEAD            0
#define PARTICLE_STATE_ALIVE           1

#define PARTICLE_TYPE_FICKER           0
#define PARTICLE_TYPE_FADE             1

#define PARTICLE_COLOR_RED             0
#define PARTICLE_COLOR_GREEN           1
#define PARTICLE_COLOR_BLUE            2
#define PARTICLE_COLOR_WHITE           3

#define MAX_PARTICLES                  128

#define INDEX_WORLD_X                  8
#define INDEX_WORLD_Y                  9

#define COLOR_RED_START                32
#define COLOR_RED_END                  47
#define COLOR_GREEN_START              96
#define COLOR_GREEN_END                111
#define COLOR_BLUE_START               144
#define COLOR_BLUE_END                 159
#define COLOR_WHITE_START              16
#define COLOR_WHITE_END                31

#define MAX_SOUNDS                256
#define DM_NUM_SEGMENTS           64
#define DSVOLTODB(vol)           ((DWORD)(-30*(100-vol)))

#define PI                       ((float)3.141592654f)
#define	PI2                      ((float)6.283185307f)
#define PI_DIV_2                 ((float)1.570796327f)
#define PI_DIV_4                 ((float)0.785398163f)
#define PI_INV                   ((float)0.318309886f)

#define FIXP16_SHIFT              16
#define FIXP16_MAG                65536
#define FIXP16_DP_MASK            0x0000ffff
#define FIXP16_WP_MASK            0xffff0000
#define FIXP16_ROUND_UP           0x00008000

#define FIXP28_SHIFT              28   //用于1/z
#define FIXP22_SHIFT              22   //用于透视修正

//非常小的常量
#define EPSILON_E2                (float)(1E-2)
#define EPSILON_E3                (float)(1E-3)
#define EPSILON_E4                (float)(1E-4)
#define EPSILON_E5                (float)(1E-5)
#define EPSILON_E6                (float)(1E-6)

//用于参数化直线交点的常量
#define PARAM_LINE_NO_INIERSECT             0
#define PARAM_LINE_INTERSECT_IN_SEGMENT     1
#define PARAM_LINE_INTERSECT_OUT_SEGMENT    2
#define PARAM_LINE_INTERSECT_EVERYWHERE     3

enum TRI4DV1Attr
{
	TRI4DV1_ATTR_2SIDED=0x0001,
	TRI4DV1_ATTR_TRANSPARENT=0x0002,
	TRI4DV1_ATTR_RGB16=0x0004,
	TRI4DV1_ATTR_RGB24=0x0008,
	TRI4DV1_ATTR_SHADE_MODE_CONSTANT=0x0010,
	TRI4DV1_ATTR_SHADE_MODE_FLAT=0x0020,
	TRI4DV1_ATTR_SHADE_MODE_GOURAUD=0x0040,
	TRI4DV1_ATTR_SHADE_MODE_FASTPHONG=0x0080,
	TRI4DV1_ATTR_SHADE_MODE_TEXTURE=0X0100,
	TRI4DV1_ATTR_ENABLE_MATERIAL=0X0200,  //在光照系统中使用材质
	TRI4DV1_ATTR_DISABLE_MATERIAL=0X0400,  //在光照系统中使用自身颜色
	TRI4DV1_ATTR_MIPMAP=0X0800         //用于支持mipmapping的新特性
};

enum TRI4DV1State
{
	TRI4DV1_STATE_NULL=0X0000,
	TRI4DV1_STATE_ACTIVE=0x0001,
	TRI4DV1_STATE_CLIPPED=0x0002,
	TRI4DV1_STATE_BACKFACE=0x0004,
	TRI4DV1_STATE_LIT=0X0008
};

enum OBJMODEL4DV1Attr
{
	OBJMODEL4DV1_ATTR_SINGLE_FRAME=0X0001,
	OBJMODEL4DV1_ATTR_MULTI_FRAME=0X0002,
	OBJMODEL4DV1_ATTR_TEXTURES=0X0004,
	OBJMODEL4DV1_ATTR_MIPMAP=0X0008   //物体是否具有mip纹理链
};
enum OBJMODEL4DV1State
{
	OBJMODEL4DV1_STATE_NULL=0X0000,
	OBJMODEL4DV1_STATE_ACTIVE=0x0001,
	OBJMODEL4DV1_STATE_VISIBLE=0x0002,
	OBJMODEL4DV1_STATE_CULLED=0x0004
};

enum VERTEXAttr
{
	VERTEX_ATTR_NULL=0X0000,
	VERTEX_ATTR_POINT=0X0001,
	VERTEX_ATTR_NORMAL=0X0002,
	VERTEX_ATTR_TEXTURE=0X0004
};

#define CULLED_X_PLANE           0X0001
#define CULLED_Y_PLANE           0X0002
#define CULLED_Z_PLANE           0X0003
#define CULLED_XYZ_PLANES        0X0001|0X0002|0X0003

#define CLIP_X_PLANE             0x0001
#define CLIP_Y_PLANE			 0X0002
#define CLIP_Z_PLANE		     0X0004
#define CLIP_XYZ_PLANE           (CLIP_X_PLANE|CLIP_Y_PLANE|CLIP_Z_PLANE)

#define CLIP_CODE_GZ   0x0001    // z > z_max
#define CLIP_CODE_LZ   0x0002    // z < z_min
#define CLIP_CODE_IZ   0x0004    // z_min < z < z_max

#define CLIP_CODE_GX   0x0001    // x > x_max
#define CLIP_CODE_LX   0x0002    // x < x_min
#define CLIP_CODE_IX   0x0004    // x_min < x < x_max

#define CLIP_CODE_GY   0x0001    // y > y_max
#define CLIP_CODE_LY   0x0002    // y < y_min
#define CLIP_CODE_IY   0x0004    // y_min < y < y_max

#define CLIP_CODE_NULL 0x0000

#define CAM_ROTATE_XYZ           0
#define CAM_ROTATE_XZY           1
#define CAM_ROTATE_YXZ           2
#define CAM_ROTATE_YZX           3
#define CAM_ROTATE_ZYX           4
#define CAM_ROTATE_ZXY           5

#define CAM_MODEL_EULER          0X0001
#define CAM_MODEL_UVN	         0X0002

#define CAM_PROJ_NORMAL          0X0003
#define CAM_PROJ_SCREEN			 0X0004
#define CAM_PROJ_FOV90           0X0005

#define UVN_MODE_SIMPLE          0X0006
#define UVN_MODE_SPHERICAL		 0X0007

#define MAX_OBJMODEL4DV1_VERTICES      4096
#define MAX_OBJMODEL4DV1_TRIANGLES     8192
#define MAX_RENDERLIST4DV1_TRIANGLES   32768

#define TRANSFORM_LOCAL_ONLY            0
#define TRANSFORM_TRANS_ONLY            1
#define TRANSFORM_LOCAL_TO_TRANS        2
//////////////////////////////////////////////////////////////////////////
//词法分析器
#define PARSER_STRIP_EMPTY_LINES        1 //去除所有空行
#define PARSER_LEAVE_EMPTY_LINES        2  //留下空行
#define PARSER_STRIP_WS_ENDS            4
#define PARSER_LEAVE_WS_ENDS            8
#define PARSER_STRIP_COMMENTS           16
#define PARSER_LEAVE_COMMENTS           32

#define PARSER_BUFFER_SIZE              256 //数据缓冲区大小
#define PARSER_MAX_COMMENT              16  //注释字符串的最大长度

#define PARSER_DEFAULT_COMMENT          '#'

#define PATTERN_TOKEN_FLOAT             'f'
#define PATTERN_TOKEN_INT               'i'
#define PATTERN_TOKEN_STRING			's'
#define PATTERN_TOKEN_LITERAL           '\''


//状态机
#define PATTERN_STATE_INIT              0
#define PATTERN_STATE_RESTART           1
#define PATTERN_STATE_FLOAT             2
#define PATTERN_STATE_INT               3
#define PATTERN_STATE_LITERAL           4
#define PATTERN_STATE_STRING            5
#define PATTERN_STATE_NEXT				6

#define PATTERN_STATE_MATCH             7
#define PATTERN_STATE_END               8


#define PATTERN_MAX_ARGS                16
#define PATTERN_BUFFER_SIZE             256


#define VERTEX_FLAGS_INVERT_X               0x00000001   
#define VERTEX_FLAGS_INVERT_Y               0x00000002   
#define VERTEX_FLAGS_INVERT_Z               0x00000004   
#define VERTEX_FLAGS_SWAP_YZ                0x00000008   //转变右手坐标系到左手坐标系
#define VERTEX_FLAGS_SWAP_XZ                0x00000010   
#define VERTEX_FLAGS_SWAP_XY                0x00000020
#define VERTEX_FLAGS_INVERT_WINDING_ORDER   0x00000040  

#define VERTEX_FLAGS_TRANSFORM_LOCAL        0x00000080
#define VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD  0x00000100

#define VERTEX_FLAGS_INVERT_TEXTURE_U       0x00000200
#define VERTEX_FLAGS_INVERT_TEXTURE_V       0X00000400
#define VERTEX_FLAGS_INVERT_SWAP_UV         0X00000800

#define VERTEX_FLAGS_OVERRIDE_MASK          0X0000f000
#define VERTEX_FLAGS_OVERRIDE_CONSTANT      0X00001000
#define VERTEX_FLAGS_OVERRIDE_FLAG          0X00002000
#define VERTEX_FLAGS_OVERRIDE_GOURAUD       0X00004000
#define VERTEX_FLAGS_OVERRIDE_TEXTURE       0X00008000

#define USE_MIPMAP                          0X00010000  //启用mipmapping

#define WALL_SPLIT_ID                       2046

#define RASTERIZER_ACCURATE                 0
#define RASTERIZER_FAST                     1
#define RASTERIZER_FASTEST                  2

//用于纹理映射
#define TRI_TYPE_NONE			  0
#define TRI_TYPE_FLAT_TOP         1
#define TRI_TYPE_FLAT_BOTTOM      2
#define TRI_TYPE_FLAGMASK         3
#define TRI_TYPE_GENERAL          4
#define INTERP_LHS                0
#define INTERP_RHS                1
#define PERTRI_MAX_VERTICES       6

#define MD2_MAGIC_NUM             (('I')+('D'<<8)+('P'<<16)+('2'<<24))
#define MD2_VERSION               8
#define MD2_NUM_ANIMATIONS        20
#define MD2_MAX_FRAME             198
#define MD2_ANIM_LOOP             0
#define MD2_ANIM_SINGLE_SHOT      1

//MD2动画状态
#define MD2_ANIM_STATE_STANDING_IDLE      0  // model is standing and idling
#define MD2_ANIM_STATE_RUN                1  // model is running
#define MD2_ANIM_STATE_ATTACK             2  // model is firing weapon/attacking
#define MD2_ANIM_STATE_PAIN_1             3  // model is being hit version 1
#define MD2_ANIM_STATE_PAIN_2             4  // model is being hit version 2
#define MD2_ANIM_STATE_PAIN_3             5  // model is being hit version 3
#define MD2_ANIM_STATE_JUMP               6  // model is jumping
#define MD2_ANIM_STATE_FLIP               7  // model is using hand gestures :)
#define MD2_ANIM_STATE_SALUTE             8  // model is saluting
#define MD2_ANIM_STATE_TAUNT              9  // model is taunting
#define MD2_ANIM_STATE_WAVE               10 // model is waving at someone
#define MD2_ANIM_STATE_POINT              11 // model is pointing at someone
#define MD2_ANIM_STATE_CROUCH_STAND       12 // model is crouching and idling
#define MD2_ANIM_STATE_CROUCH_WALK        13 // model is walking while crouching
#define MD2_ANIM_STATE_CROUCH_ATTACK      14 // model is firing weapon/attacking with crouching
#define MD2_ANIM_STATE_CROUCH_PAIN        15 // model is being hit while crouching
#define MD2_ANIM_STATE_CROUCH_DEATH       16 // model is dying while crouching
#define MD2_ANIM_STATE_DEATH_BACK         17 // model is dying while falling backward
#define MD2_ANIM_STATE_DEATH_FORWARD      18 // model is dying while falling forward
#define MD2_ANIM_STATE_DEATH_SLOW         19 // model is dying slowly (any direction)

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//	CSSD | RRRR | GGGG | BBBB
//  C为RGB/索引颜色标记
//  D为单面标记
//  RRRR\GGGG和BBBB为RGB模式下红色、绿色和蓝色分量
//  GGGGBBBB为8位颜色索引模式的颜色索引
//  位掩码
//////////////////////////////////////////////////////////////////////////
#define PLX_RGB_MASK        0X8000
#define PLX_SHADE_MODE_MASK 0x6000
#define PLX_2SIDED_MASK     0X1000
#define PLX_COLOR_MASK      0x0fff

#define PLX_COLOR_MODE_RGB_FLAG      0X8000
#define PLX_COLOR_MODE_INDEXED_FLAG  0x0000

#define PLX_2SIDED_FLAG    0X1000
#define PLX_1SIDED_FLAG    0X0000

#define PLX_SHADE_MODE_PURE_FLAG 0X0000
#define PLX_SHADE_MODE_FLAT_FLAG 0X2000
#define PLX_SHADE_MODE_GOURAUD_FLAG 0X4000
#define PLX_SHADE_MODE_PHONG_FLAG 0X6000

enum MATV1Attr
{
	MATV1_ATTR_2SIDED=0X0001,
	MATV1_ATTR_TRANSPARENT=0X0002,
	MATV1_ATTR_RGB16=0X0004,
	MATV1_ATTR_RGB24=0X0008,
	MATV1_ATTR_SHADE_MODE_CONSTANT=0X020,
	MATV1_ATTR_SHADE_MODE_FLAT=0X0040,
	MATV1_ATTR_SHADE_MODE_GOURAUD=0X0080,
	MATV1_ATTR_SHADE_MODE_FASTPHONG=0X0100,
	MATV1_ATTR_SHADE_MODE_TEXTURE=0X0200
};

#define MATV1_STATE_ACTIVE          0X0001

#define MAX_MATERIALS				256

#define MAX_MODELS                  256

enum LIGHTV1Attr
{
	LIGHTV1_ATTR_AMBIENT=0X0001,     //环境光
	LIGHTV1_ATTR_DIFFUSE=0x0002,     //漫反射
	LIGHTV1_ATTR_SPECULAR=0X0004,    //镜面反射
	LIGHTV1_ATTR_EMISSION=0X0008,    //自发光
	LIGHTV1_ATTR_INFINITE=0X0010,   //无穷远光源
	LIGHTV1_ATTR_POINT=0X0020,      //点光源
	LIGHTV1_ATTR_SIMPLE_SPOTLIGHT=0X0040,
	LIGHTV1_ATTR_COMPLEX_SPOTLIGHT=0X0080
};
#define LIGHTV1_STATE_ON		1
#define LIGHTV1_STATE_OFF		0

#define SORT_RENDERLIST_NEARZ    0
#define SORT_RENDERLIST_FARZ     1
#define SORT_RENDERLIST_AVGZ     2

typedef struct RGBAV1_TYP
{
	union
	{
		int rgba;
		UCHAR M[4];
		struct
		{
			UCHAR a,b,g,r;
		};
	};
}RGBAV1,*LPRGBAV1;

//Z缓存
#define ZBUFFER_ATTR_16BIT        16
#define ZBUFFER_ATTR_32BIT        32

//1/z缓存
#define FIXP28_SHIFT              28  //用于1/z缓存
#define FIXP22_SHIFT              22  //用于透视修正纹理映射中使用的u/z、v/z

//用于控制渲染函数状态的属性
enum RENDERAttr
{
	RENDER_ATTR_NOBUFFER                     =0X00000001,  //不使用z缓存
	RENDER_ATTR_ZBUFFER                      =0X00000002,  //使用z缓存
	RENDER_ATTR_INVZBUFFER                   =0X00000004,  //使用1/z缓存
	RENDER_ATTR_MIPMAP                       =0X00000008,  //使用mipmapping纹理链
	RENDER_ATTR_ALPHA                        =0X00000010,  //启用alpha混合
	RENDER_ATTR_BILERP                       =0X00000020,  //启用双线性滤波（只适用于固定着色和放射纹理映射）
	RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE   =0X00000040,  //用于放射纹理映射
	RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT  =0X00000080,  //用于完美透视修正纹理映射
	RENDER_ATTR_TEXTURE_PERSPECTIVE_LINEAR   =0X00000100,  //用于线性分段透视纹理映射
	RENDER_ATTR_TEXTURE_PERSPECTIVE_HYBRID1  =0X00000200,  //根据距离混合使用仿射纹理映射和线性分段纹理映射
	RENDER_ATTR_WRITETHRUZBUFFER             =0X00000400,
	RENDER_ATTR_WRITETHRUINVZBUFFER          =0X00000800
};


#define KEYDOWN(vk_code)  ((GetAsyncKeyState(vk_code) & 0x8000)? 1:0)
#define KEYUP(vk_code)    ((GetAsyncKeyState(vk_code) & 0x8000) ? 0:1)

#define _RGB16BIT555(r,g,b)        ((b & 31)+((g & 31)<<5)+((r & 31)<<10))
#define _RGB16BIT565(r,g,b)        ((b & 31)+((g & 63)<<5)+((r & 31)<<11))
#define _RGB24BIT(a,r,g,b)         ((b)+(g<<8)+(r<<16))
#define _RGB32BIT(a,r,g,b)         (b+(g<<8)+(r<<16)+(a<<24))
#define _RGBA32BIT(r,g,b,a)        ((a)+((b)<<8)+((g)<<16)+((r)<<24))
#define _RGB565FROM16BIT(RGB,r,g,b)   {*r=((RGB>>11)&0x1f);*g=((RGB>>5)&0x3f);*b=(RGB&0x1f);}
#define _RGB555FROM16BIT(RGB,r,g,b)   {*r=((RGB>>10)&0x1f);*g=((RGB>>5)&0x1f);*b=(RGB&0x1f);}


#define SET_BIT(word,bit_flag)     ((word)=( (word) | (bit_flag)))
#define RESET_BIT(word,bit_flag)   ((word)=( (word) & (~bit_flag)))

#define DDRAW_INIT_STRUCT(ddstruct)  {memset(&ddstruct,0,sizeof(ddstruct));ddstruct.dwSize=sizeof(ddstruct);}

#ifndef DSBCAPS_CTRLDEFAULT
#define DSBCAPS_CTRLDEFAULT (DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME )
#endif

#define ANG_TO_RAD(angle)        ((angle)*PI/180.0)
#define RAD_TO_ANG(radian)       ((radian)*180.0/PI)

#define RAND_RANGE(x,y)          ( (x) +(rand() % ((y)-(x)+1)) )


//灯光闪烁
typedef struct BLINKER_TYP
{
	int color_index;
	PALETTEENTRY on_color;
	PALETTEENTRY off_color;
	int on_time;          //保持开的帧数
	int off_time;         //保持关的帧数
	int counter;         //用于状态转变的计数
	int state;           //-1 off,1 on,0 dead
}BLINKER,*LPBLINKER;

typedef struct VERTEX2D_TYP
{
	float x;
	float y;
}VERTEX2D,*LPVERTEX2D;


inline BOOL FCMP(float a,float b)
{
	if (fabs(a-b)<EPSILON_E2)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
inline void swap(int& a,int& b) {int temp;temp=a;a=b;b=temp;}
inline void swap(float& a,float& b) {float temp;temp=a;a=b;b=temp;}
inline int fabs(int& x){if(x<0)x=-x;return x;}
inline void MemSetWORD(void* dest, USHORT data, int count)
{
	_asm
	{
		mov edi,dest;
		mov ecx,count;
		mov ax,data;
		rep stosw;
	}
}
inline void MemSetDWORD(void* dest, DWORD data, int count)
{
	_asm
	{
		mov edi,dest;
		mov ecx,count;
		mov Eax,data;
		rep stosd;
	}
}


inline void PadName(char* filename,char* extension,char* padstring,int num)
{
	char buf[80];
	wsprintf(padstring,"%s0000.%s",filename,extension);
	int digestIndex=strlen(filename)+4;
	_itoa_s(num,buf,10);
	memcpy(padstring+digestIndex-strlen(buf),buf,strlen(buf));
}

inline float Distance2D(float x,float y)
{
	x=fabs(x);
	y=fabs(y);
	int mn=(int)min(x,y);
	return (x+y -(mn>>1)-(mn>>2)+(mn>>4));
}

inline float Distance3D(float fx, float fy, float fz)
{
	int x = fabs(fx) * 1024;
	int y = fabs(fy) * 1024;
	int z = fabs(fz) * 1024;
	if (y < x) std::swap(x,y);
	if (z < y) std::swap(y,z);
	if (y < x) std::swap(x,y);
	int dist = ( z + 11 * (y >> 5) + (x >> 2) );
	return  ((float)(dist >> 10));
}

inline void CreateMathTable()
{
	int i;
	float val=-1;
	for (i=0;i<=360;i++)
	{
		float angle=ANG_TO_RAD(i);
		cosTable[i]=cos(angle);
		sinTable[i]=sin(angle);

		val=(val>1)?1:val;
		invcosTable[i]=RAD_TO_ANG(acos(val));
		//计算下一个[-1,1]的值
		val+=((float)2/(float)360);
	}
	invcosTable[i]=invcosTable[i-1];
}

void CreateAlphaTable(int alphaLevels,USHORT AlphaTable[][65535]);

//////////////////////////////////////////////////////////////////////////
//通过查找表，线性插值，计算小数负数的正弦，余弦
//////////////////////////////////////////////////////////////////////////

inline float FastSin(float theta)
{
	theta=fmodf(theta,360);    //将角度转化为0~359
	if (theta<0)
	{
		theta+=360.0;
	}
	//提取角度的整数部分和小数部分，以便进行插值
	int IntgerTheta=(int)theta;
	float fracTheta=theta-IntgerTheta;

	return (sinTable[IntgerTheta]+fracTheta*(sinTable[IntgerTheta+1]-sinTable[IntgerTheta]));
}
inline float FastCos(float theta)
{
	theta=fmodf(theta,360);    //将角度转化为0~359
	if (theta<0)
	{
		theta+=360.0;
	}
	//提取角度的整数部分和小数部分，以便进行插值
	int IntgerTheta=(int)theta;
	float fracTheta=theta-IntgerTheta;

	return (cosTable[IntgerTheta]+fracTheta*(cosTable[IntgerTheta+1]-cosTable[IntgerTheta]));
}

inline float FastInvcos(float x)
{
	return invcosTable[int((x+1)*(float)180)];
}

inline BOOL ColliCheck(int x1, int y1, int w1, int h1, 
					   int x2, int y2, int w2, int h2)
{
	int width1  = (w1>>1) - (w1>>3);
	int height1 = (h1>>1) - (h1>>3);

	int width2  = (w2>>1) - (w2>>3);
	int height2 = (h2>>1) - (h2>>3);

	int cx1 = x1 + width1; 
	int cy1 = y1 + height1;

	int cx2 = x2 + width2;
	int cy2 = y2 + height2;

	int dx = abs(cx2 - cx1);
	int dy = abs(cy2 - cy1);

	if (dx < (width1+width2) && dy < (height1+height2))
		return TRUE;
	else

		return FALSE;
}

#endif