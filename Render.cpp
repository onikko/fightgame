#include "Render.h"

void CBiginRender::Render(){
	m_lpd3ddevice->BeginScene();
	m_lpd3ddevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);
}

void CEndRender::Render(){
	HRESULT  hr;
	m_lpd3ddevice->EndScene();
	hr = m_lpd3ddevice->Present(NULL, NULL, NULL, NULL);	// バックバッファからプライマリバッファへ転送
	if (hr != D3D_OK){
		m_lpd3ddevice->Reset(&m_d3dpp);
	}
}