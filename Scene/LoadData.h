#pragma once
#include <thread>
#include "../CBillBoard.h"
#include "../Utilities/CDebugFont.h"
#include "../Utilities/GameObj.h"

class CLoadData : public CGameObj
{
private:
	CDebugFont* m_font;
	TCHAR str[256];
	RECT rc;
	float debug;
	int cnt;
	int flag;
	int brightness;
public:
	CLoadData();
	virtual ~CLoadData();
protected:
	static thread m_loadthread;
	static thread m_loadplayer1;
	static thread m_loadplayer2;
	static bool complete;
	static list<ListData> temp;

	virtual void Render();
};

class thread_guard{
	thread& t;
public:
	explicit thread_guard(thread& _t) : t(_t){}
	~thread_guard(){
		if (t.joinable()){
			t.join();
		}
	}
	thread_guard(thread_guard const&) = delete;
	thread_guard& operator = (thread_guard const&) = delete;
};

