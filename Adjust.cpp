#include "Adjust.h"

#define MIN_DIST 0.7f
CAdjust::CAdjust()
{
	printf("’²®ƒNƒ‰ƒX‚ð“Ç‚Ýž‚Ý‚Ü‚µ‚½\n");
}


CAdjust::~CAdjust()
{
}

void CAdjust::Init()
{
	player = (CPlayer*)FindItemBox("player");
	enemy = (CEnemy*)FindItemBox("enemy");
}

void CAdjust::Update()
{
	D3DXMATRIX& m0 = player->GetMatrixWorld();
	D3DXMATRIX& m1 = enemy->GetMatrixWorld();
	D3DXVECTOR3 p0 = D3DXVECTOR3(m0._41, 0, m0._43);
	D3DXVECTOR3 p1 = D3DXVECTOR3(m1._41, 0, m1._43);
	center = (p0 + p1)*0.5f;
	unit = p0 - p1;
	dist = D3DXVec3Length(&unit);
	D3DXVec3Normalize(&unit, &unit);

	if (dist<MIN_DIST) {
		D3DXVECTOR3 vpos = unit*(MIN_DIST - dist)*0.5f;
		player->SetTranslation(vpos.x, vpos.y, vpos.z);
		enemy->SetTranslation(-vpos.x, -vpos.y, -vpos.z);
	}
	player->MakeTotalMatrix();
	enemy->MakeTotalMatrix();
}

void CAdjust::Render()
{

}