#pragma once
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <xnamath.h>
#include <vector>
using namespace std;


// Define vertex structure
struct POS_VERT
{
	XMFLOAT3 Pos;
	XMFLOAT2 Texture;
};


class Bitmap
{
public:
	Bitmap(string filename, float x, float y, float width, float height); // pass in filename of font, device and context);
	~Bitmap();

	// render all strings at once
	void RenderText();


	float m_x, m_y, m_width, m_height; 

private:
	ID3D11Device*           pD3DDevice;
	ID3D11DeviceContext*    pImmediateContext;

	ID3D11ShaderResourceView*	pTexture;
	ID3D11SamplerState*			pSampler;

	POS_VERT vertices[6];	// system memory vertex list, to be copied to vertex buffer

	ID3D11Buffer*			pVertexBuffer;
	ID3D11VertexShader*		pVShader;
	ID3D11PixelShader*		pPShader;
	ID3D11InputLayout*		pInputLayout;

	ID3D11DepthStencilState* pDepthEnabledStencilState;		// state to turn on Z buffer
	ID3D11DepthStencilState* pDepthDisabledStencilState;	// state to turn off Z buffer
};

