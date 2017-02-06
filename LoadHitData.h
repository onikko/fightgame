#pragma once
#include <stdio.h> 
#include <string> 
#include <Windows.h>
using namespace std;

class CLoadHitData
{
private:
	FILE* fp;
	char line[MAX_PATH];
	char pos_name[MAX_PATH];
	char pos_radius[MAX_PATH];
	int cnt1;
	int cnt2;
public:
	CLoadHitData(const char* text);
	~CLoadHitData();
	string LoadName();
	float LoadRadius();
	void Reset();
};

class CReadHitData
{
private:
	FILE* fp;
	char buff[MAX_PATH];
public:
	CReadHitData(const char* text);
	~CReadHitData();
	void ReadData(char* buf);
};

