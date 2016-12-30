#pragma once
#include "Utilities/GameObj.h"
class CBiginRender : public CGameObj
{
protected:
	void Render();
};

class CEndRender : public CGameObj
{
protected:
	void Render();
};