#include "HitCheck.h"
#include "Chr/CCharacterMng.h"

CHitCheck::CHitCheck()
{
	printf("hitcheckÇì«Ç›çûÇ›Ç‹ÇµÇΩ\n");
}

CHitCheck::~CHitCheck()
{

}

void CHitCheck::Init()
{
	player = (CPlayer*)FindItemBox("player");
	enemy = (CEnemy*)FindItemBox("enemy");
	debug = (CDebugMgr*)FindItemBox("Debug");
	flag = false;
	hitflag = false;
	id = 0;
	damege = 0.0f;
}

void CHitCheck::Update()
{
	/*for (int i = 0; i < HIT_DEFENSE_POS; i++){
		if (HitTest(player->GetAttack()[player->GetID()].world._41, player->GetAttack()[player->GetID()].world._42, 
			player->GetAttack()[player->GetID()].world._43, player->GetAttack()[player->GetID()].radius,
			enemy->GetDefenseData()[i].world._41, enemy->GetDefenseData()[i].world._42, 
			enemy->GetDefenseData()[i].world._43, enemy->GetDefenseData()[i].radius) && hitflag == false)
		{
			enemy->GetGaugeData()->Damage(player->GetDamege());
			hitflag = true;
		}
	}*/
	if (Hit(player->GetSample()._41, player->GetSample()._42, player->GetSample()._11, player->GetSample()._22, 0.5f, 0.5f,
		player->GetSample1()._41, player->GetSample1()._42, player->GetSample1()._11, player->GetSample1()._22, 0.5f, 0.5f))
	{
		hitflag = true;
	}
}

void CHitCheck::Render()
{
	if (hitflag == true){
		sprintf((char *)font, "ìñÇΩÇË");
		debug->Draw(font, 5, 150);
	}
	hitflag = false;
}

bool CHitCheck::HitTest(float in_x, float in_y, float in_z, float in_radius, float in_x1, float in_y1, float in_z1, float in_radius1)
{
	float x = in_x;
	float y = in_y;
	float z = in_z;
	float r = in_radius;
	float x1 = in_x1;
	float y1 = in_y1;
	float z1 = in_z1;
	float r1 = in_radius1;
	if ((x1 - x)*(x1 - x) + (y1 - y)*(y1 - y) + (z1 - z)*(z1 - z) <= (r + r1)*(r + r1)){
		return true;
	}
	return false;
}

bool CHitCheck::Hit(float center_x, float center_y, float scale_x, float scale_y, float offset_x, float offset_y,
	float center_x1, float center_y1, float scale_x1, float scale_y1, float offset_x1, float offset_y1)
{
	float x = center_x - scale_x - offset_x;
	float y = center_y - scale_y - offset_y;
	float width = center_x + scale_x + offset_x;
	float height = center_y + scale_y + offset_y;
	float x_1 = center_x1 - scale_x1 - offset_x1;
	float y_1 = center_y1 - scale_y1 - offset_y1;
	float width_1 = center_x1 + scale_x1 + offset_x1;
	float height_1 = center_y1 + scale_y1 + offset_y1;
	if ((x < x_1 + width_1) && (x_1 < x + width) &&
		(y < y_1 + height_1) && (y_1 < y + height)){
		return true;
	}
	return false;
}

void CHitCheck::Set(int in_id, float in_damege, bool in_flag)
{
	id = in_id;
	damege = in_damege;
	flag = in_flag;
}