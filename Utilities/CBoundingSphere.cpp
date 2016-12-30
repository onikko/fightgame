#include "CBoundingSphere.h"

void CBoundingSphere::CalculateBoundingSphere(const LPD3DXMESH lpmesh, const LPDIRECT3DDEVICE9 lpdevice){
	HITDATA atari;
	LPD3DXMESH clonemesh;

	LPDIRECT3DVERTEXBUFFER9 lpVertexBuffer;
	int numvertices;

	float *x;
	float *y;
	float *z;

	float minx, miny, minz;
	float maxx, maxy, maxz;
	float cx, cy, cz;

	HRESULT hr;

	BYTE* data;
	float* pfloat;

	void* pVertices = NULL;

	hr = lpmesh->CloneMeshFVF(D3DXMESH_MANAGED, (D3DFVF_XYZ), lpdevice, &clonemesh);
	if (FAILED(hr)){
		MessageBox(NULL, "Clone mesh create error", "Clone mesh create error", MB_OK);
	}
	else{
		numvertices = clonemesh->GetNumVertices();
		clonemesh->GetVertexBuffer(&lpVertexBuffer);

		hr = lpVertexBuffer->Lock(0, 0, (void**)&data, D3DLOCK_READONLY);
		if (hr == D3D_OK){
			x = new float[numvertices];
			y = new float[numvertices];
			z = new float[numvertices];

			pfloat = (float *)data;
			for (int i = 0; i < numvertices; i++){
				x[i] = *pfloat++;
				y[i] = *pfloat++;
				z[i] = *pfloat++;
			}
			for (int i = 0; i < numvertices; i++){
				if (i == 0){
					minx = maxx = x[i];
					miny = maxy = y[i];
					minz = maxz = z[i];
				}
				if (minx >= x[i]) minx = x[i];
				if (miny >= y[i]) miny = y[i];
				if (minz >= z[i]) minz = z[i];
				if (maxx <= x[i]) maxx = x[i];
				if (maxy <= y[i]) maxy = y[i];
				if (maxz <= z[i]) maxz = z[i];
			}

			cx = (maxx + minx) / 2;
			cy = (maxy + miny) / 2;
			cz = (maxz + minz) / 2;

			float distance;
			float maxdistance = 0;

			for (int i = 0; i < numvertices; i++){
				distance = (x[i] - cx)*(x[i] - cx) + (y[i] - cy)*(y[i] - cy) + (z[i] - cz)*(z[i] - cz);
				if (maxdistance < distance) maxdistance = distance;
			}

			maxdistance = sqrtf(maxdistance);
			delete[] z;
			delete[] y;
			delete[] x;
			lpVertexBuffer->Unlock();
			atari.minx = minx;
			atari.miny = miny;
			atari.minz = minz;
			atari.maxx = maxx;
			atari.maxy = maxy;
			atari.maxz = maxz;
			atari.cx = cx;
			atari.cy = cy;
			atari.cz = cz;
			atari.r = maxdistance;
		}
	}
	clonemesh->Release();
	m_hitdata = atari;
}
