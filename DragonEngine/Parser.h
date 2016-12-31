#pragma once
#include "datatype.h"

class CParser
{
public:
	CParser(void);
	~CParser(void);

	int Reset();
	int OpenFile(char* filename);
	int CloseFile();
	char* GetLine(int mode);
	int SetComment(char* string);
	int PatternMatch(char* string,char* pattern,...);
	
	//辅助函数
	int IsInt(char* istring);
	float IsFloat(char* fstring);
	char* StringRTrim(char* string);
	char* StringLTrim(char* string);
	int ReplaceChars(char* stringIn,char* stringOut,char* replaceChars,char repChar,int CaseFlag=1);
	int StripChars(char* stringIn,char* stringOut,char* stripChars,int CaseFlag=1);
	int GetFileNameFromPath(char* filePath,char* fileName);
public:
	FILE* fstream;                        //文件指针
	char buffer[PARSER_BUFFER_SIZE];      // 缓存区
	int length;                           //当前行长
	int numLines;                         //处理了多少行
	char comment[PARSER_MAX_COMMENT];     //单行注释字符串

	//模式匹配参数存储空间
	//函数PatternMatch()返回时，匹配的内容将存储在这些数组中
	char pstrings[PATTERN_MAX_ARGS][PATTERN_BUFFER_SIZE];    //字符串
	int  num_pstrings;
	float pfloats[PATTERN_MAX_ARGS];                   //浮点数
	int num_pfloats;
	int pints[PATTERN_MAX_ARGS];                //整数
	int num_pints;
};
