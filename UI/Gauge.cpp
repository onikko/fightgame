#include "Gauge.h"

void CGauge::Reset()
{
	Value = PrevValue = 0;
	Value1 = 1.0f / GAUGE_TIME;
	Wait = 0;
}

void CGauge::Damage(float v)
{
	if (Value>0 && Value1 <= 0) {
		Value -= v;
		if (Value<0) Value = 0;
		Value1 = (Value - PrevValue) / GAUGE_TIME;
		Wait = GAUGE_TIME;
	}
}

void CGauge::Move()
{
	if (Wait>0) {
		Wait--;
	}
	else {
		if (Value1<0) {
			if (PrevValue>Value) {
				PrevValue += Value1;
			}
			else {
				PrevValue = Value;
				Value1 = 0;
			}
		}
		else
			if (Value1>0) {
				if (Value<1) {
					Value += Value1;
				}
				else {
					Value = PrevValue = 1;
					Value1 = 0;
				}
			}
	}
}

void CGauge::Draw()
{
	CTexture* t = DummyTexture;
	float sw = (float)m_width;
	float sh = (float)m_height;
	float x = sw*0.42f;
	float y = sh*0.04f;
	float w = -sw*0.4f;
	float h = sh*0.03f;
	if (Index == 1){
		x = sw - x;
		w = -w;
	}
	m_lpd3ddevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	t->Draw(x, y - h*0.05f, w, h*0.05f, 0, 0, 1, 1, D3DCOLOR_ARGB(255, 255, 255, 255));
	t->Draw(x, y + h, w, h*0.05f, 0, 0, 1, 1, D3DCOLOR_ARGB(255, 255, 255, 255));
	t->Draw(x, y, w, h, 0, 0, 1, 1, D3DCOLOR_ARGB(100, 0, 0, 0));
	if (PrevValue > Value){
		t->Draw(x + Value*w, y, (PrevValue - Value)*w, h, 0, 0, 1, 1, D3DCOLOR_ARGB(220, 255, 10, 10));
	}
	t->Draw(x, y, Value*w, h, 0, 0, 1, 1, D3DCOLOR_ARGB(255, 120, 220, 100));
	m_lpd3ddevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}