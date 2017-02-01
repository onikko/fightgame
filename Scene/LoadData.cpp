#include "LoadData.h"
#include "../Utilities/CDebugFont.h"

bool CLoadData::complete;
list<ListData> CLoadData::temp;
thread CLoadData::m_loadthread;
thread CLoadData::m_loadplayer1;
thread CLoadData::m_loadplayer2;

#define DEBUG_X 5
#define DEBUG_Y 130
#define LOADFONT_X 590
#define LOADFONT_Y 610
#define DOT_X 730
#define DOT_Y 615
unsigned MemoryUsageMegaBytes();

CLoadData::CLoadData()
{
	// staticデータをリセット
	complete = false;
	temp.clear();
	m_font = new CDebugFont(25);
	debug = 0;
	cnt = 0;
	flag = 1;
	// 変数の初期化
	brightness = 255;
}

CLoadData::~CLoadData()
{
	m_loadthread.joinable();
	thread_guard tg(m_loadthread);
	//m_loadthread.joinable();
	SAFE_DELETE(m_font);
	// 中断されたのであれば、追加予定だったオブジェクトを破棄する
	if (complete == false){
		list<ListData>::iterator i = temp.begin();
		while (i != temp.end()){
			if ((*i).autodelete == true) delete (*i).gameobj;
			i = temp.erase(i);
		}
	}
}

void CLoadData::Render()
{
	if (complete == false){
		// バックグラウンド処理がまだ行われている
		brightness -= 2;
		if (brightness < 0) brightness = 255;
		//確認用
		sprintf((char *)str, "デバッグ：%5.2f", debug);
		SetRect(&rc, DEBUG_X, DEBUG_Y, 0, 0);
		m_font->Draw(str, -1, &rc, D3DCOLOR_ARGB(255, 0, 0, 0));
		//ロードフォント
		sprintf((char *)str, "Now Loading");
		SetRect(&rc, LOADFONT_X, LOADFONT_Y, 0, 0);
		m_font->Draw(str, -1, &rc, D3DCOLOR_ARGB(255, 0, 0, 0));
		switch (flag)
		{
		case 1:
			sprintf((char *)str, "・");
			SetRect(&rc, DOT_X, DOT_Y, 0, 0);
			m_font->Draw(str, -1, &rc, D3DCOLOR_ARGB(255, 0, 0, 0));
			break;
		case 2:
			sprintf((char *)str, "・・");
			SetRect(&rc, DOT_X, DOT_Y, 0, 0);
			m_font->Draw(str, -1, &rc, D3DCOLOR_ARGB(255, 0, 0, 0));
			break;
		case 3:
			sprintf((char *)str, "・・・");
			SetRect(&rc, DOT_X, DOT_Y, 0, 0);
			m_font->Draw(str, -1, &rc, D3DCOLOR_ARGB(255, 0, 0, 0));
			break;
		}
		sprintf((char *)str, "現在のメモリ使用量：%u MB\n", MemoryUsageMegaBytes());
		SetRect(&rc, DEBUG_X, DEBUG_Y + 25, 0, 0);
		debug++;
		cnt++;
		if (cnt > 40 * flag)	flag++;
		if (flag>4){
			flag = 0;
			cnt = 0;
		}
	}
	else{
		thread_guard tg(m_loadthread);
		//m_loadthread.joinable();
		// バックグラウンドで用意されたクラスをタスクに追加する
		list<ListData>::iterator i;
		for (i = temp.begin(); i != temp.end(); i++){
			CGameObj::Instance().AppendObj(*i);
		}
		// 自分自身は用済みなので破棄
		GAME.RemoveObj(this);
	}
}

unsigned MemoryUsageMegaBytes(){
	MEMORYSTATUSEX m = { sizeof m };
	GlobalMemoryStatusEx(&m);
	return (unsigned)(((512 * 1024) + (m.ullTotalVirtual - m.ullAvailVirtual)) / (1024 * 1024));
}