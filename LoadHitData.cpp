#include "LoadHitData.h"


CLoadHitData::CLoadHitData(const char* text)
{
	fp = fopen(text, "rt");
	cnt1 = 0;
	cnt2 = 0;
	memset(pos_name, '\0', MAX_PATH);
	memset(pos_radius, '\0', MAX_PATH);
}

string CLoadHitData::LoadName()
{
	memset(pos_name, '\0', MAX_PATH);
	fgets(line, MAX_PATH, fp);
	for (cnt1 = 0; cnt1 < sizeof(line); cnt1++){
		if (line[cnt1] == ','){
			pos_name[cnt1] = '\0';
			cnt1++;
			break;
		}
		pos_name[cnt2] = line[cnt1];
		cnt2++;
	}
	return pos_name;
}

float CLoadHitData::LoadRadius()
{
	memset(pos_radius, '\0', MAX_PATH);
	cnt2 = 0;
	for (cnt1 = cnt1; cnt1 < sizeof(line); cnt1++){
		if (line[cnt1] == '\n'){
			pos_radius[cnt1] = '\0';
			cnt1++;
			break;
		}
		pos_radius[cnt2] = line[cnt1];
		cnt2++;
	}
	return (float)atof(pos_radius);
}

void CLoadHitData::Reset()
{
	cnt1 = 0;
	cnt2 = 0;
	memset(pos_name, '\0', MAX_PATH);
	memset(pos_radius, '\0', MAX_PATH);
}


CLoadHitData::~CLoadHitData()
{
	fclose(fp);
}
