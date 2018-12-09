#include "Bitmap.h"
#include "Game.h"


Bitmap::Bitmap(string filename, float x, float y, float width, float height)
{

	m_x = x;
	m_y = y; 
	m_width = width;
	m_height = height;

	// create 6 vertices with given size and xy position
	vertices[0].Pos.x = m_x;
	vertices[0].Pos.y = m_y;
	vertices[1].Pos.x = m_x + m_width;
	vertices[1].Pos.y = m_y;
	vertices[2].Pos.x = m_x + m_width;
	vertices[2].Pos.y = m_y - m_height;
	vertices[3].Pos.x = m_x;
	vertices[3].Pos.y = m_y;
	vertices[4].Pos.x = m_x + m_width;
	vertices[4].Pos.y = m_y - m_height;
	vertices[5].Pos.x = m_x;
	vertices[5].Pos.y = m_y - m_height;

	// set all z to 1.0 to avoid being clipped
	vertices[0].Pos.z = 1.0;
	vertices[1].Pos.z = 1.0;
	vertices[2].Pos.z = 1.0;
	vertices[3].Pos.z = 1.0;
	vertices[4].Pos.z = 1.0;
	vertices[5].Pos.z = 1.0;

	// set correct texture coordinates for letter
	vertices[0].Texture.x = 0;
	vertices[0].Texture.y = 0;
	vertices[1].Texture.x = 1.0;
	vertices[1].Texture.y = 0;
	vertices[2].Texture.x = 1.0;
	vertices[2].Texture.y = 1.0;
	vertices[3].Texture.x = 0;
	vertices[3].Texture.y = 0;
	vertices[4].Texture.x = 1.0;
	vertices[4].Texture.y = 1.0;
	vertices[5].Texture.x = 0;
	vertices[5].Texture.y = 1.0;

	pD3DDevice = TheGame::Singleton()->getDevice();
	pImmediateContext = TheGame::Singleton()->getContext();

	// Set up and create vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;										// Used by CPU and GPU
	bufferDesc.ByteWidth = sizeof(vertices);									// Total size of buffer,
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;							// Use as a vertex buffer
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;							// Allow CPU access
	HRESULT hr = pD3DDevice->CreateBuffer(&bufferDesc, NULL, &pVertexBuffer);	// Create the buffer

	if (FAILED(hr)) exit(0);

	// Load and compile pixel and vertex shaders - use vs_5_0 to target DX11 hardware only
	ID3DBlob *VS, *PS, *error;
	hr = D3DX11CompileFromFile("text2d_shaders.hlsl", 0, 0, "TextVS", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	if (error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))exit(0);
	}

	hr = D3DX11CompileFromFile("text2d_shaders.hlsl", 0, 0, "TextPS", "ps_4_0", 0, 0, 0, &PS, &error, 0);

	if (error != 0)
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr)) exit(0);
	}

	// Create shader objects
	hr = pD3DDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVShader);
	if (FAILED(hr)) exit(0);

	hr = pD3DDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPShader);
	if (FAILED(hr)) exit(0);

	// Create and set the input layout object
	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};

	hr = pD3DDevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &pInputLayout);
	if (FAILED(hr)) exit(0);

	// Load in the font texture from given filename
	hr = D3DX11CreateShaderResourceViewFromFile(pD3DDevice, filename.c_str(), NULL, NULL, &pTexture, NULL);
	if (FAILED(hr)) exit(0);

	// Create sampler for texture
	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = pD3DDevice->CreateSamplerState(&sampler_desc, &pSampler);

	// Create 2 depth stencil states to turn Z buffer on and off
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = pD3DDevice->CreateDepthStencilState(&depthStencilDesc, &pDepthDisabledStencilState);
	if (FAILED(hr)) exit(0);

	depthStencilDesc.DepthEnable = true;
	hr = pD3DDevice->CreateDepthStencilState(&depthStencilDesc, &pDepthEnabledStencilState);
	if (FAILED(hr)) exit(0);
}


Bitmap::~Bitmap()
{
	if (pDepthDisabledStencilState) pDepthDisabledStencilState->Release();
	if (pDepthEnabledStencilState) pDepthEnabledStencilState->Release();
	if (pTexture) pTexture->Release();
	if (pSampler) pSampler->Release();
	if (pVertexBuffer) pVertexBuffer->Release();
	if (pVShader) pVShader->Release();
	if (pPShader) pPShader->Release();
	if (pInputLayout) pInputLayout->Release();
}

void Bitmap::RenderText() {

	// Copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	pImmediateContext->Map(pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);	// Lock the buffer to allow writing
	memcpy(ms.pData, vertices, sizeof(vertices));					// Copy the data -  only upload those that are used
	pImmediateContext->Unmap(pVertexBuffer, NULL);

	// set all rendering states
	pImmediateContext->PSSetSamplers(0, 1, &pSampler);
	pImmediateContext->PSSetShaderResources(0, 1, &pTexture);
	pImmediateContext->VSSetShader(pVShader, 0, 0);
	pImmediateContext->PSSetShader(pPShader, 0, 0);
	pImmediateContext->IASetInputLayout(pInputLayout);

	UINT stride = sizeof(POS_VERT);
	UINT offset = 0;
	pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// turn off Z buffer so text always on top
	pImmediateContext->OMSetDepthStencilState(pDepthDisabledStencilState, 1);

	// draw all added characters
	pImmediateContext->Draw(6, 0);

	// turn on Z buffer so other rendering can use it
	pImmediateContext->OMSetDepthStencilState(pDepthEnabledStencilState, 1);

}