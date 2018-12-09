#pragma once
#include "Model.h"

class SkyBox : public Model
{
public:
	SkyBox();
	~SkyBox();

	void Draw(XMVECTOR* CameraPos, XMMATRIX* view, XMMATRIX* projection);


private:

	ID3D11RasterizerState*		g_pRasterSolid = 0; 
	ID3D11RasterizerState*		g_pRasterSkyBox = 0;
	ID3D11DepthStencilState*	g_pDepthWriteSolid = 0;
	ID3D11DepthStencilState*	g_pDepthWriteSkyBox = 0; 

	ID3D11Device*			m_pD3DDevice;
	ID3D11DeviceContext*	m_pImmediateContext;

	//Camera*						pos;
	//SceneNode*						skybox; 
};

