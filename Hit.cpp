#include "Hit.h"
#include "Utilities/SafeDelete.h"

Hit::Hit(int in_size)
{
	BoneName = new string[in_size];
	Radius = new float[in_size];
	BoneIndex = new int[in_size];
	world = new D3DXMATRIX[in_size];
	pos = new D3DXVECTOR3[in_size];
	size = in_size;
}


Hit::~Hit()
{
	SAFE_DELETE_ARRAY(pos);
	SAFE_DELETE_ARRAY(world);
	SAFE_DELETE_ARRAY(BoneIndex);
	SAFE_DELETE_ARRAY(Radius);
	SAFE_DELETE_ARRAY(BoneName);
}



