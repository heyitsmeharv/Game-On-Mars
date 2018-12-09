#include "SkyBox.h"
#include "Game.h"
#include "SceneNode.h"

struct SKYBOX_CONSTANT_BUFFER
{
	XMMATRIX WorldViewProjection;	// 64 bytes ( 4 x 4 = 16 floats x 4 bytes)
}; // TOTAL SIZE = 64 bytes

SkyBox::SkyBox() : Model()
{

	HRESULT hr;

	m_pImmediateContext = TheGame::Singleton()->getContext();
	m_pD3DDevice = TheGame::Singleton()->getDevice();

	D3D11_RASTERIZER_DESC skyboxDesc;
	ZeroMemory(&skyboxDesc, sizeof(skyboxDesc));

	skyboxDesc.CullMode = D3D11_CULL_BACK ;
	skyboxDesc.FillMode = D3D11_FILL_SOLID;
	hr = m_pD3DDevice->CreateRasterizerState(&skyboxDesc, &g_pRasterSolid);

	skyboxDesc.CullMode = D3D11_CULL_FRONT;
	hr = m_pD3DDevice->CreateRasterizerState(&skyboxDesc, &g_pRasterSkyBox );

	D3D11_DEPTH_STENCIL_DESC skyboxDepth;
	ZeroMemory(&skyboxDepth, sizeof(skyboxDepth));

	skyboxDepth.DepthEnable = true; 
	skyboxDepth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	
	skyboxDepth.DepthFunc = D3D11_COMPARISON_LESS;
	hr = m_pD3DDevice->CreateDepthStencilState(&skyboxDepth, &g_pDepthWriteSolid);

	skyboxDepth.DepthEnable = false;
	
	skyboxDepth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	hr = m_pD3DDevice->CreateDepthStencilState(&skyboxDepth, &g_pDepthWriteSkyBox);

}


SkyBox::~SkyBox()
{
	if (g_pRasterSolid) g_pRasterSolid->Release();
	if (g_pRasterSkyBox) g_pRasterSkyBox->Release(); 
	if (g_pDepthWriteSolid) g_pDepthWriteSolid->Release(); 
	if (g_pDepthWriteSkyBox) g_pDepthWriteSkyBox->Release(); 
}

void SkyBox::Draw(XMVECTOR* CameraPos, XMMATRIX* view, XMMATRIX* projection)
{
	SKYBOX_CONSTANT_BUFFER skybox_cb_values;

	XMMATRIX m_Local_World_Matrix = XMMatrixIdentity();

	m_Local_World_Matrix *= XMMatrixScaling(m_scale, m_scale, m_scale);

	m_Local_World_Matrix *= XMMatrixTranslation(XMVectorGetX(*CameraPos), XMVectorGetY(*CameraPos), XMVectorGetZ(*CameraPos));

	skybox_cb_values.WorldViewProjection = m_Local_World_Matrix * (*view) * (*projection);
	//skybox_cb_values.WorldViewProjection = gameCam->GetFixedViewMatrix() * projection;

	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &skybox_cb_values, 0, 0);
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampler);
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture);

	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);

	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	m_pImmediateContext->RSSetState(g_pRasterSkyBox);
	m_pImmediateContext->OMSetDepthStencilState(g_pDepthWriteSkyBox, 0); 

	m_pObject->Draw();

	m_pImmediateContext->RSSetState(g_pRasterSolid);
	m_pImmediateContext->OMSetDepthStencilState(g_pDepthWriteSolid, 0);
}
