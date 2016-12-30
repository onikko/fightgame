#pragma once
#include	<d3dx9.h>
#include "../Utilities/SmartPointer.h"

/// 描画インターフェース
class Drawable3D {
public:
	virtual ~Drawable3D(){};
	virtual void Draw(D3DXMATRIX world, D3DLIGHT9* light, D3DXMATRIX view, D3DXMATRIX proj) = 0;
};
