#pragma once
#include	<Windows.h>
#include	<tchar.h>
#include	<d3d9.h>
#include	<d3dx9.h>
#include	<DxErr.h>
#include	<list>
#include	<string>
#include	<hash_map>
#include "../Texture.h"

using namespace std;
class CGameObj;
class CCamaraTPS;
class CDebugMgr;
class Versus;

#define NAME_SIZE 32
struct ListData{
	int priority;
	char name[NAME_SIZE];
	CGameObj* gameobj;
	bool autodelete;
};

class CGameObj
{
private:
	static D3DDISPLAYMODE				m_disp;				// ディスプレイモード
	static int							m_adapter;			// ディスプレイアダプタ番号
	static list<ListData> objlist;
	static bool it_move;
	static list<ListData>::iterator it_task;
	static hash_map<string, CGameObj*> itembox;
public:
	CGameObj* FindObj(char* name);
	virtual ~CGameObj(){}
	static CGameObj &Instance(void)
	{
		static CGameObj instance;
		return instance;
	}

	bool	Initialize(HWND hwnd, HINSTANCE hInstance, bool fullscreenflag, int width, int height);	// 初期処理
	void	Uninitialize();// 終了処理
	LPDIRECT3DDEVICE9	GetDXDevice() const {							// デバイスオブジェクトＧＥＴ
		return m_lpd3ddevice;
	}
	D3DPRESENT_PARAMETERS GetDXD3dpp() const{							// プレゼンテーションパラメータＧＥＴ
		return m_d3dpp;
	}
	HWND GetHWND() const{
		return m_hwnd;
	}
	int GetWindows_Width() const{
		return m_width;
	}
	int GetWindows_Height() const{
		return m_height;
	}
	static void AppendObj(ListData &listdata);
	static void AppendObj(CGameObj *obj, int priority, bool autodelete);
	static void AppendObj(CGameObj *obj, int priority, LPCSTR name, bool autodelete);
	static void RemoveObj(CGameObj *target);
	static void RemoveObj(int p_begin, int p_end);
	static void AppendItemBox(LPCSTR name, CGameObj* obj);
	static void RemoveItemBox(LPCSTR name);
	static void ResetItemBox();
	static CGameObj* FindItemBox(LPCSTR name);
	void DoInputTasks();
	void DoUpdateTasks();
	void DoRenderTasks();
protected:
	static LPDIRECT3D9					m_lpd3d;			// DIRECT3D8オブジェクト
	static LPDIRECT3DDEVICE9			m_lpd3ddevice;		// DIRECT3DDEVICE8オブジェクト
	static D3DPRESENT_PARAMETERS		m_d3dpp;			// プレゼンテーションパラメータ
	static LPD3DXSPRITE					m_lpsprite;
	static HINSTANCE					m_hinstance;
	static HWND							m_hwnd;
	static CTexture*					DummyTexture;
	static int							m_width;			// バックバッファＸサイズ
	static int							m_height;			// バックバッファＹサイズ
	static bool							m_Paused;
	static bool							m_PrevInput;
	static bool							m_UpdateFlag;
	static bool							m_TopView;
	static bool							m_GameEndFlag;
	virtual void	Init(){}
	virtual void	Input(){}
	virtual void	Update(){}
	virtual void	Render(){}
};

// 色
#define COL_WHITE D3DCOLOR_ARGB(255, 255, 255, 255)
#define COL_LGRAY D3DCOLOR_ARGB(255, 192, 192, 192)
#define COL_MGRAY D3DCOLOR_ARGB(255, 128, 128, 128)
#define COL_DGRAY D3DCOLOR_ARGB(255, 64, 64, 64)
#define COL_BLACK D3DCOLOR_ARGB(255, 0, 0, 0)
#define COL_SHADE D3DCOLOR_ARGB(128, 0, 0, 0)

#define GAME CGameObj::Instance() 