#pragma once
#include "Utilities/GameObj.h"
#include "Utilities/CDirect3DXFile.h"
#include "SkinMesh/SkinMesh.h"
#include "Chr/CPlayer.h"
#include "Camera/CamaraTPS.h"


#define BACKGROUND_PRIORITY 10000
class CBackground : public CGameObj
{
private:
	CDirect3DXFile* m_stage;
	D3DXMATRIX m_MatWorld;
public:
	CBackground();
	CBackground(CDirect3DXFile*);
	~CBackground();
	CDirect3DXFile *Get(){
		return m_stage;
	}
protected:
	virtual void Init();
	virtual void Render();
};

