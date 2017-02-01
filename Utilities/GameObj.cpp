#include "GameObj.h"
#include "SafeDelete.h"

LPDIRECT3D9				CGameObj::m_lpd3d = NULL;
LPDIRECT3DDEVICE9		CGameObj::m_lpd3ddevice = NULL;
D3DPRESENT_PARAMETERS	CGameObj::m_d3dpp;
D3DDISPLAYMODE			CGameObj::m_disp;
HWND					CGameObj::m_hwnd = NULL;
LPD3DXSPRITE			CGameObj::m_lpsprite = NULL;
HINSTANCE				CGameObj::m_hinstance = NULL;
int						CGameObj::m_adapter = NULL;
int						CGameObj::m_width = NULL;
int						CGameObj::m_height = NULL;
CTexture*				CGameObj::DummyTexture = NULL;
bool					CGameObj::m_Paused = false;
bool					CGameObj::m_PrevInput = false;
bool					CGameObj::m_UpdateFlag = true;
bool					CGameObj::m_TopView = false;
bool					CGameObj::m_GameEndFlag = false;

list<ListData> CGameObj::objlist;
bool CGameObj::it_move = NULL;
list<ListData>::iterator CGameObj::it_task;
hash_map<string, CGameObj*> CGameObj::itembox;

bool CGameObj::Initialize(HWND hwnd, HINSTANCE hInstance, bool fullscreenflag, int width, int height)
{
	HRESULT	hr;
	this->m_hwnd = hwnd;
	this->m_hinstance = hInstance;
	m_lpd3d = Direct3DCreate9(D3D_SDK_VERSION);	// Direct3D9�C���^�[�t�F�[�X�擾
	if (m_lpd3d == NULL){
		MessageBox(hwnd, "ERROR!!", "DirectXDevice�������G���[", MB_OK);
		return false;
	}

	// �A�_�v�^�̌��݂̃f�B�X�v���C���[�h���擾����
	if (FAILED(m_lpd3d->GetAdapterDisplayMode(m_adapter, &m_disp))){
		MessageBox(hwnd, "ERROR!!", "DirectX3DDevice�������G���[", MB_OK);
		return false;
	}

	memset(&m_d3dpp, 0, sizeof(m_d3dpp));			// �[���N���A
	m_d3dpp.BackBufferFormat = m_disp.Format;	// ���݂̃r�b�g�[�x
	m_d3dpp.BackBufferWidth = width;			// �o�b�N�o�b�t�@�̕����Z�b�g
	m_d3dpp.BackBufferHeight = height;			// �o�b�N�o�b�t�@�̍������Z�b�g
	m_d3dpp.BackBufferCount = 1;				// �o�b�N�o�b�t�@�̐�
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
	m_d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;	// �o�b�N�o�b�t�@�����b�N�\�ɂ���

	if (fullscreenflag)	m_d3dpp.Windowed = FALSE;	// �t���X�N���[�����[�h
	else					m_d3dpp.Windowed = TRUE;	// �E�C���h�E���[�h

	// �y�o�b�t�@�̎����쐬�i�r�b�g�[�x24�j
	m_d3dpp.EnableAutoDepthStencil = 1;
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;	// VSYNC��҂��Ȃ�

	// �f�o�C�X�쐬
	hr = m_lpd3d->CreateDevice(m_adapter,
		D3DDEVTYPE_HAL,
		hwnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
		&m_d3dpp,
		&m_lpd3ddevice);
	if (hr != D3D_OK){
		hr = m_lpd3d->CreateDevice(m_adapter,
			D3DDEVTYPE_HAL,
			hwnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
			&m_d3dpp,
			&m_lpd3ddevice);
		if (hr != D3D_OK){
			hr = m_lpd3d->CreateDevice(m_adapter,
				D3DDEVTYPE_REF,
				hwnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
				&m_d3dpp,
				&m_lpd3ddevice);
			if (hr != D3D_OK)		return(false);
		}
	}

	DummyTexture = new CTexture(m_lpd3ddevice, 1, 1);
	D3DXVECTOR4 color(1, 1, 1, 1);
	DummyTexture->Fill(&color);

	// ���C�g�L��
	m_lpd3ddevice->SetRenderState(D3DRS_LIGHTING, true);
	// �J�����O������
	//m_lpd3ddevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_lpd3ddevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	// �����Z�b�g
	m_lpd3ddevice->SetRenderState(D3DRS_AMBIENT, 0xffffffff);
	m_lpd3ddevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);    //�A���t�@�u�����f�B���O�̗L����
	m_lpd3ddevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);  //SRC�̐ݒ�
	m_lpd3ddevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_height = height;
	m_width = width;

	if (FAILED(D3DXCreateSprite(m_lpd3ddevice, &m_lpsprite))){
		MessageBox(hwnd, "ERROR!!", "SpriteObj�̍쐬�Ɏ��s���܂���", MB_OK);
		return false;
	}

	return(true);
}

void CGameObj::Uninitialize()
{
	SAFE_RELEASE(m_lpsprite);
	SAFE_RELEASE(m_lpd3ddevice);
	SAFE_RELEASE(m_lpd3d);
	// ���X�g��new�Œǉ������f�[�^�����ׂď���
	list<ListData>::iterator i;
	for (i = objlist.begin(); i != objlist.end(); i++)
	{
		if ((*i).autodelete == true) delete (*i).gameobj;
	}
}

void CGameObj::AppendObj(ListData &listdata){
	if (listdata.gameobj == NULL) return;
	// �������֐������s
	listdata.gameobj->Init();
	// ���X�g�̐擪�ɒǉ�
	if (objlist.size() == 0){
		objlist.push_front(listdata);
		return;
	}
	// �I�u�W�F�N�g�̏��ʂɏ]���}��
	list<ListData>::iterator i;
	for (i = objlist.begin(); i != objlist.end(); i++)
	{
		if ((*i).priority > listdata.priority){
			objlist.insert(i, listdata);
			return;
		}
	}
	// �Ԃɓ����Ƃ��낪�Ȃ���΁A���X�g�̍Ō�ɒǉ�
	objlist.push_back(listdata);
}

void CGameObj::AppendObj(CGameObj *obj, int priority, bool autodelete){
	AppendObj(obj, priority, NULL, autodelete);
}

void CGameObj::AppendObj(CGameObj *obj, int priority, LPCSTR name, bool autodelete){
	ListData ld;
	ld.priority = priority;
	if (name != NULL)	strcpy_s(ld.name, NAME_SIZE, name);
	else				ZeroMemory(ld.name, NAME_SIZE*sizeof(char));
	ld.gameobj = obj;
	ld.autodelete = autodelete;
	AppendObj(ld);
}

void CGameObj::RemoveObj(CGameObj *target)
{
	list<ListData>::iterator i = objlist.begin();
	while (i != objlist.end())
	{
		if ((*i).gameobj == target){
			// ��������C�e���[�^�����݃^�X�N�������̂��̂ł����
			// �t���O�𗧂ĂāA�^�X�N�̃C�e���[�^�����ֈړ�
			if (it_task == i){
				it_move = true;
				it_task++;
			}

			// �Q�[���I�u�W�F�N�g�����
			if (i->autodelete) delete i->gameobj;

			objlist.erase(i);
			return;
		}
		else{
			i++;
		}
	}
}

void CGameObj::RemoveObj(int p_begin, int p_end)
{
	list<ListData>::iterator i = objlist.begin();
	while (i != objlist.end()){
		if ((*i).priority > p_end){
			break;
		}
		else if ((*i).priority >= p_begin){
			if (it_task == i){
				it_move = true;
				it_task++;
			}

			if (i->autodelete) delete i->gameobj;
			i = objlist.erase(i);
		}
		else{
			i++;
		}
	}
}

void CGameObj::DoInputTasks()
{
	it_task = objlist.begin();

	it_move = false;
	while (it_task != objlist.end()){
		it_task->gameobj->Input();
		if (it_move == false){
			// ���Ɉړ�
			it_task++;
		}
		else{
			// �t���O�����Z�b�g
			it_move = false;
		}
	}
}
void CGameObj::DoUpdateTasks()
{
	it_task = objlist.begin();

	it_move = false;
	while (it_task != objlist.end()){
		it_task->gameobj->Update();
		if (it_move == false){
			// ���Ɉړ�
			it_task++;
		}
		else{
			// �t���O�����Z�b�g
			it_move = false;
		}
	}
}
void CGameObj::DoRenderTasks()
{
	it_task = objlist.begin();

	it_move = false;
	while (it_task != objlist.end()){
		it_task->gameobj->Render();
		// ���Ɉړ�
		if (it_move == false){
			it_task++;
		}
		// �t���O�����Z�b�g
		else{
			it_move = false;
		}

	}
}

CGameObj* CGameObj::FindObj(char* name){
	list<ListData>::iterator i;
	for (i = objlist.begin(); i != objlist.end(); i++){
		if (strcmp((*i).name, name) == 0)	return (*i).gameobj;
	}
	return NULL;
}

void CGameObj::AppendItemBox(LPCSTR name, CGameObj* obj){
	itembox.insert(make_pair(name, obj));
}

void CGameObj::RemoveItemBox(LPCSTR name){
	itembox.erase(name);
}

void CGameObj::ResetItemBox(){
	itembox.clear();
}

CGameObj* CGameObj::FindItemBox(LPCSTR name){
	hash_map<string, CGameObj*>::iterator i;
	i = itembox.find(name);
	if (i == itembox.end()){
		printf("%s�ɑΉ�����f�[�^�����݂��܂���", name);
		return NULL;
	}
	return i->second;
}
