#include "StdAfx.h"
#include "ErrorLog.h"
#include "datatype.h"



extern HWND hWnd;

CErrorLog::CErrorLog(void)
{
	hFile=NULL;
}

CErrorLog::~CErrorLog(void)
{
}

void CErrorLog::CreateErrorFile()
{
	char filename[]={"errorlog.txt"};
	if (fopen_s(&hFile,filename,"w"))
	{
		MessageBox(hWnd,"打开错误日志文件失败！","",MB_OK);
	}
	_timeb timebuffer;
	char* timeline;
	char timestring[280];
	_ftime_s(&timebuffer);            //获得当前本地时间
	timeline=ctime(&(timebuffer.time));  //把时间值转换成字符串
	wsprintf(timestring,"%.19s.%hu, %s",timeline,timebuffer.millitm,&timeline[20]);
	WriteError("\nOpening Error Output File (%s) on %s\n",filename,timestring);
	fclose(hFile);  //以增加模式重新打开文件
	if (!fopen(filename,"a+"))
	{
		MessageBox(hWnd,"打开错误日志文件失败！","",MB_OK);
	}
}

void CErrorLog::WriteError(char* string,...)
{
	char buf[80];
	va_list arglist;
	if (!string||!hFile)
	{
		MessageBox(hWnd,"写日志失败！","",MB_OK);
	}

	va_start(arglist,string);
	vsprintf_s(buf,string,arglist);
	va_end(arglist);
	
	fprintf(hFile,buf);

	fflush(hFile);
}

void CErrorLog::CloseFile()
{

	if (hFile)
	{
		WriteError("\nClose the errorlog file output.");
		fclose(hFile);
		hFile=NULL;
	}
}

void CErrorLog::PrintVector2(const CVector2 &v,char* name)
{
	if (!hFile)
	{
		MessageBox(hWnd,"Vector2写日志失败！","",MB_OK);
		return ;
	}

	WriteError("\n%s=[",name);
	for (int i=0;i<2;i++)
	{
		WriteError("%f, ",v.M[i]);
	}
	WriteError("]");
}

void CErrorLog::PrintVector3(const CVector3 &v,char* name)
{
	if (!hFile)
	{
		MessageBox(hWnd,"Vector3写日志失败！","",MB_OK);
		return ;
	}

	WriteError("\n%s=[",name);
	for (int i=0;i<3;i++)
	{
		WriteError("%f, ",v.M[i]);
	}
	WriteError("]");
}

void CErrorLog::PrintVector4(const CVector4 &v,char* name)
{
	if (!hFile)
	{
		MessageBox(hWnd,"Vector4写日志失败！","",MB_OK);
		return ;
	}

	WriteError("\n%s=[",name);
	for (int i=0;i<4;i++)
	{
		WriteError("%f, ",v.M[i]);
	}
	WriteError("]");
}

void CErrorLog::PrintMatrix2x2(const CMatrix2x2 &m,char* name)
{
	if (!hFile)
	{
		MessageBox(hWnd,"Matrix2x2写日志失败！","",MB_OK);
		return ;
	}

	WriteError("\n%s=[",name);
	for (int i=0;i<4;i++)
	{
		WriteError("%f, ",m.M[i]);
	}
	WriteError("]");
}

void CErrorLog::PrintMatrix3x3(const CMatrix3x3& m,char* name)
{
	if (!hFile)
	{
		MessageBox(hWnd,"Matrix3x3写日志失败！","",MB_OK);
		return ;
	}

	WriteError("\n%s=[",name);
	for (int r=0;r<3;r++)
	{
		for (int c=0;c<3;c++)
		{
			WriteError("%f, ",m.M[r][c]);
		}
		WriteError("\n");
	}
	WriteError("]");
}

void CErrorLog::PrintMatrix4x4(const CMatrix4x4& m,char* name)
{
	if (!hFile)
	{
		MessageBox(hWnd,"Matrix4x4写日志失败！","",MB_OK);
		return ;
	}

	WriteError("\n%s=[",name);
	for (int r=0;r<4;r++)
	{
		for (int c=0;c<4;c++)
		{
			WriteError("%f, ",m.M[r][c]);
		}
		WriteError("\n");
	}
	WriteError("]");
}

void CErrorLog::PrintQuat(const CQuaternion & q,char* name)
{
	if (!hFile)
	{
		MessageBox(hWnd,"Quat写日志失败！","",MB_OK);
		return ;
	}

	WriteError("\n%s=[",name);
	for (int i=0;i<4;i++)
	{
		WriteError("%f, ",q.M[i]);
	}
	WriteError("]");
}
