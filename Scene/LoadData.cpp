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
	// static�f�[�^�����Z�b�g
	complete = false;
	temp.clear();
	m_font = new CDebugFont(25);
	debug = 0;
	cnt = 0;
	flag = 1;
	// �ϐ��̏�����
	brightness = 255;
}

CLoadData::~CLoadData()
{
	m_loadthread.joinable();
	thread_guard tg(m_loadthread);
	//m_loadthread.joinable();
	SAFE_DELETE(m_font);
	// ���f���ꂽ�̂ł���΁A�ǉ��\�肾�����I�u�W�F�N�g��j������
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
		// �o�b�N�O���E���h�������܂��s���Ă���
		brightness -= 2;
		if (brightness < 0) brightness = 255;
		//�m�F�p
		sprintf((char *)str, "�f�o�b�O�F%5.2f", debug);
		SetRect(&rc, DEBUG_X, DEBUG_Y, 0, 0);
		m_font->Draw(str, -1, &rc, D3DCOLOR_ARGB(255, 0, 0, 0));
		//���[�h�t�H���g
		sprintf((char *)str, "Now Loading");
		SetRect(&rc, LOADFONT_X, LOADFONT_Y, 0, 0);
		m_font->Draw(str, -1, &rc, D3DCOLOR_ARGB(255, 0, 0, 0));
		switch (flag)
		{
		case 1:
			sprintf((char *)str, "�E");
			SetRect(&rc, DOT_X, DOT_Y, 0, 0);
			m_font->Draw(str, -1, &rc, D3DCOLOR_ARGB(255, 0, 0, 0));
			break;
		case 2:
			sprintf((char *)str, "�E�E");
			SetRect(&rc, DOT_X, DOT_Y, 0, 0);
			m_font->Draw(str, -1, &rc, D3DCOLOR_ARGB(255, 0, 0, 0));
			break;
		case 3:
			sprintf((char *)str, "�E�E�E");
			SetRect(&rc, DOT_X, DOT_Y, 0, 0);
			m_font->Draw(str, -1, &rc, D3DCOLOR_ARGB(255, 0, 0, 0));
			break;
		}
		sprintf((char *)str, "���݂̃������g�p�ʁF%u MB\n", MemoryUsageMegaBytes());
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
		// �o�b�N�O���E���h�ŗp�ӂ��ꂽ�N���X���^�X�N�ɒǉ�����
		list<ListData>::iterator i;
		for (i = temp.begin(); i != temp.end(); i++){
			CGameObj::Instance().AppendObj(*i);
		}
		// �������g�͗p�ς݂Ȃ̂Ŕj��
		GAME.RemoveObj(this);
	}
}

unsigned MemoryUsageMegaBytes(){
	MEMORYSTATUSEX m = { sizeof m };
	GlobalMemoryStatusEx(&m);
	return (unsigned)(((512 * 1024) + (m.ullTotalVirtual - m.ullAvailVirtual)) / (1024 * 1024));
}