// flv2h264.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


//自己测试可以用，原文链接http://blog.163.com/zhujiatc@126/blog/static/18346382013413112230525/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

#define HTON16(x)  ((x>>8&0xff)|(x<<8&0xff00))
#define HTON24(x)  ((x>>16&0xff)|(x<<16&0xff0000)|x&0xff00)
#define HTON32(x)  ((x>>24&0xff)|(x>>8&0xff00)|\
	(x << 8 & 0xff0000) | (x << 24 & 0xff000000))
#define STR(x) (x.c_str())
#define FCUR(x) (ftell(x))
#define FSEEK(x,f) (fseek(f,x,SEEK_CUR))
#define FSET(x,f) (fseek(f,x,SEEK_SET))
//#define LOG(x,f) (fprintf(f,STR(x)))
FILE *flvfile = NULL;
FILE *h264file = NULL;
unsigned int size;
bool Init();
void Clear();
bool Read8(int &i8, FILE*f);
bool Read16(int &i16, FILE*f);
bool Read24(int &i24, FILE*f);
bool Read32(int &i32, FILE*f);
bool ReadTime(int &itime, FILE*f);
bool Peek8(int &i8, FILE*f);
bool ReadHead();
void ReadBody();
char flvfilename[256] = { 0 };
char h264filename[256] = { 0 };
bool hasname = false;
int main(int argc, char**argv)
{
	strncpy(flvfilename, "d:/happyhe.flv", sizeof(flvfilename));
	strncpy(h264filename, "d:/happyhe.h264", sizeof(h264filename));
	hasname = true;
	if (!Init())
	{
		cout << "Init Err" << endl;
		return -1;
	}
	if (!ReadHead())
	{
		cout << "ReadHead Err" << endl;
		return -1;
	}
	ReadBody();
	cout << "read over" << endl;
	Clear();
	return 0;
}
bool Init()
{
	flvfile = fopen(flvfilename, "rb");
	h264file = fopen(h264filename, "wb");
	if (flvfile == NULL || h264file == NULL)
	{
		return false;
	}
	fseek(flvfile, 0, SEEK_END);
	//size = FCUR(flvfile);
	fseek(flvfile, 0, SEEK_SET);
	return true;
}
void Clear()
{
	fclose(flvfile);
	fclose(h264file);
}
bool Read8(int &i8, FILE*f)
{
	if (fread(&i8, 1, 1, f) != 1)
		return false;
	return true;
}
bool Read16(int &i16, FILE*f)
{
	if (fread(&i16, 2, 1, f) != 1)
		return false;
	i16 = HTON16(i16);
	return true;
}
bool Read24(int &i24, FILE*f)
{
	if (fread(&i24, 3, 1, f) != 1)
		return false;
	i24 = HTON24(i24);
	return true;
}
bool Read32(int &i32, FILE*f)
{
	if (fread(&i32, 4, 1, f) != 1)
		return false;
	i32 = HTON32(i32);
	return true;
}
bool Peek8(int &i8, FILE*f)
{
	if (fread(&i8, 1, 1, f) != 1)
		return false;
	fseek(f, -1, SEEK_CUR);
	return true;
}
bool ReadTime(int &itime, FILE*f)
{
	int temp = 0;
	if (fread(&temp, 4, 1, f) != 1)
		return false;
	itime = HTON24(temp);
	itime |= (temp & 0xff000000);
	return true;
}
bool ReadHead()
{
	int headlength = 0;
	int filetype = 0;
	if (!Read24(filetype, flvfile))
		return false;
	int typel = 'flv';
	int typeh = 'FLV';
	if (filetype != typeh&&filetype != typel)
	{
		printf("not flv file\n");
		return false;
	}
	FSEEK(2, flvfile);
	if (!Read32(headlength, flvfile))
		return false;
	printf("headlength:%d\n", headlength);
	/* 跳过头部长度 */
	fseek(flvfile, 0, SEEK_SET);
	FSEEK(headlength, flvfile);
	return true;
}
void H264JIEXI(int datelength);
void ReadBody()
{
	while (true)
	{
		FSEEK(4, flvfile);
		int type = 0;
		int time = 0;
		int htime = 0;
		int datelength = 0;
		int info = 0;
		char buff[256] = { 0 };
		if (!Read8(type, flvfile))
			break;
		if (!Read24(datelength, flvfile))
			break;
		if (!ReadTime(time, flvfile))
			break;
		//跳过StreamID /
		FSEEK(3, flvfile);
		if (!Peek8(info, flvfile))
			break;
		int pos = FCUR(flvfile);
		if (type == 9)
			H264JIEXI(datelength);
		FSET(pos + datelength, flvfile);
	}
}
int h264space = 0x01000000;//H264内容间隔标识00000001
void H264JIEXI(int datelength)//关键
{
	int info = 0;
	Read8(info, flvfile);
	int avctype = 0;
	Read8(avctype, flvfile);
	FSEEK(3, flvfile);
	int templength = 0;
	char*tempbuff = NULL;
	if (avctype == 0)
	{
		FSEEK(6, flvfile);
		Read16(templength, flvfile);
		printf("sssize:%d\n", templength);
		tempbuff = (char*)malloc(templength);
		fread(tempbuff, 1, templength, flvfile);
		fwrite(&h264space, 1, 4, h264file);
		fwrite(tempbuff, 1, templength, h264file);
		free(tempbuff);
		Read8(templength, flvfile);//ppsnum
		Read16(templength, flvfile);//ppssize
		printf("ppsize:%d\n", templength);
		tempbuff = (char*)malloc(templength);
		fread(tempbuff, 1, templength, flvfile);
		fwrite(&h264space, 1, 4, h264file);
		fwrite(tempbuff, 1, templength, h264file);
		free(tempbuff);
	}
	else
	{
		//	Read32(templength,flvfile);
		//	tempbuff=(char*)malloc(templength);
		//	fread(tempbuff,1,templength,flvfile);
		//	fwrite(&h264space,1,4,h264file);
		//	fwrite(tempbuff,1,templength,h264file);
		//	free(tempbuff);
		//可能存在多个nal，需全部读取
		int countsize = 2 + 3;
		while (countsize < datelength)
		{
			Read32(templength, flvfile);
			tempbuff = (char*)malloc(templength);
			fread(tempbuff, 1, templength, flvfile);
			fwrite(&h264space, 1, 4, h264file);
			fwrite(tempbuff, 1, templength, h264file);
			free(tempbuff);
			countsize += (templength + 4);
		}
	}
}
