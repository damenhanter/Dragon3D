#pragma once
#include "datatype.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix2x2.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "Quaternion.h"

class CErrorLog
{
public:
	CErrorLog(void);
	~CErrorLog(void);

	void CreateErrorFile();
	void WriteError(char* string,...);
	void CloseFile();
	void PrintVector2(const CVector2& v,char* name);
	void PrintVector3(const CVector3& v,char* name);
	void PrintVector4(const CVector4& v,char* name);
	void PrintMatrix2x2(const CMatrix2x2& m,char* name);
	void PrintMatrix3x3(const CMatrix3x3& m,char* name);
	void PrintMatrix4x4(const CMatrix4x4& m,char* name);
	void PrintQuat(const CQuaternion& q,char* name);
private:
	FILE* hFile;
};
