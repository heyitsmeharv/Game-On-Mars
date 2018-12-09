#pragma once
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <xnamath.h>
#include <windows.h>
#include "objfilemodel.h"

class Model
{
public:
	Model();
	~Model();

	int LoadObjModel(char* filename, LPCSTR hlsl);
	void Draw(XMMATRIX *world, XMMATRIX *view, XMMATRIX *projection); 
	void AddTexture(LPCSTR filename);

	void SetPos(float x, float y, float z) { m_x = x, m_y = y, m_z = z; }
	float GetPosX() { return m_x; }
	float GetPosY() { return m_y; }
	float GetPosZ() { return m_z; }

	void SetRot(float x, float y, float z) { m_xangle = x, m_yangle = y, m_zangle = z; }
	float GetRotX() { return m_xangle; }
	float GetRotY() { return m_yangle; }
	float GetRotZ() { return m_zangle; }

	void SetScale(float scale) { m_scale = scale; }
	float GetScaleX() { return m_scale; }
	float GetScaleY() { return m_scale; }
	float GetScaleZ() { return m_scale; }

	float GetHeight();
	float GetWidth(); 
	float GetDepth(); 

	void LookAt_XZY(float dx, float dz, float dy);
	void LookAt_XZ(float dx, float dz);
	void MoveForward(float distance);

	void up(float amount);
	void down(float amount);
	void left(float amount);
	void right(float amount);

	XMVECTOR GetBoundingSphereWorldSpacePosition();
	XMVECTOR GetModelCentrePoint();
	float GetBoundingSphereRadius(); 
	float GetBoundingSphereX() { return m_bounding_sphere_centre_x; }
	float GetBoundingSphereY() { return m_bounding_sphere_centre_y; }
	float GetBoundingSphereZ() { return m_bounding_sphere_centre_z; }
	bool CheckCollision(Model* collision); 


protected:

	ObjFileModel*			m_pObject;
	ID3D11VertexShader*		m_pVShader;
	ID3D11PixelShader*		m_pPShader;
	ID3D11InputLayout*		m_pInputLayout;
	ID3D11Buffer*			m_pConstantBuffer;

	ID3D11ShaderResourceView*	m_pTexture;
	ID3D11SamplerState*			m_pSampler;

	ID3D11Device*			m_pD3DDevice;
	ID3D11DeviceContext*	m_pImmediateContext;

	float m_x, m_y, m_z;
	float m_xangle, m_zangle, m_yangle;
	float m_scale;

	float m_height, m_width, m_depth; 

private:

	float m_bounding_sphere_centre_x; 
	float m_bounding_sphere_centre_y;
	float m_bounding_sphere_centre_z;
	float m_bounding_sphere_centre_radius;

	void CalculateModelCentrePoint(); 
	void CalculatedBoundingSphereRadius(); 

};

