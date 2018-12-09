#include "Model.h"
#include "Game.h"

struct MODEL_CONSTANT_BUFFER
{
	XMMATRIX WorldViewProjection;	// 64 bytes ( 4 x 4 = 16 floats x 4 bytes)
}; // TOTAL SIZE = 64 bytes


Model::Model()
{
	m_pD3DDevice = TheGame::Singleton()->getDevice();
	m_pImmediateContext = TheGame::Singleton()->getContext();

	m_pTexture = NULL;
	m_pSampler = NULL; 

	m_x = 0.0f;
	m_y = 0.0f;
	m_z = 0.0f;
	m_xangle = 0.0f;
	m_yangle = 0.0f;
	m_zangle = 0.0f;
	m_scale = 1.0f;

	

}


Model::~Model()
{
	if (m_pObject) delete m_pObject;
	if (m_pInputLayout) m_pInputLayout->Release();
	if (m_pVShader) m_pVShader->Release(); 
	if (m_pPShader) m_pPShader->Release(); 
	if (m_pConstantBuffer) m_pConstantBuffer->Release();
	if (m_pTexture) m_pTexture->Release(); 
}


int Model::LoadObjModel(char* filename, LPCSTR hlsl) {

	HRESULT hr = S_OK;

	m_pObject = new ObjFileModel(filename, m_pD3DDevice, m_pImmediateContext);

	if (m_pObject->filename == "FILE NOT LOADED") return S_FALSE;
	
	// Set up and create constant buffer
	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));
	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;	// Can use UpdateSubresource() to update
	constant_buffer_desc.ByteWidth = 64;				// MUST be a multiple of 16, calculate from CB struct
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // Use as a constant buffer

	hr = m_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &m_pConstantBuffer);

	if (FAILED(hr)){ // return error code on failure
		return hr;
	}


	// Load and compile pixel and vertex shaders - use vs_5_0 to target DX11 hardware only
	ID3DBlob *VS, *PS, *error;
	hr = D3DX11CompileFromFile(hlsl, 0, 0, "ModelVS", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	if (error != 0){ // Check for error
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr)){ // Don't fail if error is just a warning
			return hr;
		};
	}

	hr = D3DX11CompileFromFile(hlsl, 0, 0, "ModelPS", "ps_4_0", 0, 0, 0, &PS, &error, 0);

	if (error != 0){ // Check for error
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr)){ // Don't fail if error is just a warning
			return hr;
		};
	}

	// Create shader objects
	hr = m_pD3DDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &m_pVShader);

	if (FAILED(hr)){
		return hr;
	}

	hr = m_pD3DDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &m_pPShader);

	if (FAILED(hr)){
		return hr;
	}

	// Set the shader objects as active
	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);

	// Create and set the input layout object
	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }

	};

	hr = m_pD3DDevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &m_pInputLayout);

	if (FAILED(hr)){
		return hr;
	}

	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	CalculateModelCentrePoint();
	CalculatedBoundingSphereRadius();

	return S_OK;


}

void Model::Draw(XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection)
{
	MODEL_CONSTANT_BUFFER model_cb_values;
	model_cb_values.WorldViewProjection = (*world)*(*view)*(*projection);

	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &model_cb_values, 0, 0);
	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampler);
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture);

	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);

	m_pImmediateContext->IASetInputLayout(m_pInputLayout);
	
	m_pObject->Draw();
	
}



void Model::LookAt_XZ(float dx, float dz){
	dx = dx - m_x;
	dz = dz - m_z;

	m_yangle = atan2(dx, dz) * (180.0 / XM_PI); 

}

void Model::LookAt_XZY(float dx, float dz, float dy){
	dx = dx - m_x;
	dz = dz - m_z;
	dy = dy - m_y; 

	m_yangle = atan2(dx, dz) * (180.0 / XM_PI);

}


void Model::MoveForward(float distance){
	m_x += sin(m_yangle * (XM_PI / 180.0)) * distance;
	m_z += cos(m_yangle * (XM_PI / 180.0)) * distance;
	
}

void Model::up(float amount){
	m_y += amount;

}

void Model::down(float amount){

	m_y -= amount;
}

void Model::left(float amount) {

	m_x -= amount;
}

void Model::right(float amount) {

	m_x += amount; 
}

void Model::CalculateModelCentrePoint() {

	float sx = 0.0f;
	float bx = 0.0f;
	float sy = 0.0f;
	float by = 0.0f;
	float sz = 0.0f;
	float bz = 0.0f;

	for (int i = 0; i < m_pObject->numverts; i++) {

		if (m_pObject->vertices[i].Pos.x < sx) {

			sx = m_pObject->vertices[i].Pos.x;

		}
		else if (m_pObject->vertices[i].Pos.x > bx) {

			bx = m_pObject->vertices[i].Pos.x;

		}

		if (m_pObject->vertices[i].Pos.y < sy) {

			sy = m_pObject->vertices[i].Pos.y;

		}
		else if (m_pObject->vertices[i].Pos.y > by) {

			by = m_pObject->vertices[i].Pos.y;

		}
		if (m_pObject->vertices[i].Pos.z < sz) {

			sy = m_pObject->vertices[i].Pos.z;

		}
		else if (m_pObject->vertices[i].Pos.z > bz) {

			bz = m_pObject->vertices[i].Pos.z;

		}
	}

	m_bounding_sphere_centre_x = (bx - abs(sx)) * 0.5;
	m_bounding_sphere_centre_y = (by - abs(sy)) * 0.5;
	m_bounding_sphere_centre_z = (bz - abs(sz)) * 0.5; 

	m_height = by - sy;
	m_width = bx - sx;
	m_depth = bz - sz; 

}

XMVECTOR Model::GetModelCentrePoint(){

	XMVECTOR MCP;

	MCP = XMVectorSet(m_bounding_sphere_centre_x, m_bounding_sphere_centre_y, m_bounding_sphere_centre_z, 0.0);

	return MCP; 

}

void Model::CalculatedBoundingSphereRadius() {

	float radius; 
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	for (int i = 0; i < m_pObject->numverts; i++) {

		x = ((m_pObject->vertices[i].Pos.x - m_bounding_sphere_centre_x) * (m_pObject->vertices[i].Pos.x - m_bounding_sphere_centre_x));
		y = ((m_pObject->vertices[i].Pos.y - m_bounding_sphere_centre_y) * (m_pObject->vertices[i].Pos.y - m_bounding_sphere_centre_y));
		z = ((m_pObject->vertices[i].Pos.z - m_bounding_sphere_centre_z) * (m_pObject->vertices[i].Pos.z - m_bounding_sphere_centre_z));
	
		radius = sqrt(x + y + z);

		if (radius > m_bounding_sphere_centre_radius) {

			m_bounding_sphere_centre_radius = radius;
		}

	}
}

XMVECTOR Model::GetBoundingSphereWorldSpacePosition() {

	XMMATRIX world;
	XMVECTOR offset;

	world = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_xangle), XMConvertToRadians(m_yangle), XMConvertToRadians(m_zangle));
	world *= XMMatrixScaling(m_scale, m_scale, m_scale);
	world *= XMMatrixTranslation(m_x, m_y, m_z);

	offset = XMVectorSet(m_bounding_sphere_centre_x, m_bounding_sphere_centre_y, m_bounding_sphere_centre_z, 0.0);
	offset = XMVector3Transform(offset, world);

	return offset; 

}

float Model::GetBoundingSphereRadius() {

	return m_bounding_sphere_centre_radius * m_scale; 

}

bool Model::CheckCollision(Model* otherObjectCheck) {

	XMVECTOR currentObjectPos = GetBoundingSphereWorldSpacePosition();
	XMVECTOR otherObjectPos = otherObjectCheck->GetBoundingSphereWorldSpacePosition();

	if (otherObjectCheck == this) {

		return false;

	}
	else
	{

		float distanceSquared = (pow(XMVectorGetX(currentObjectPos) - XMVectorGetX(otherObjectPos), 2) +
								pow(XMVectorGetY(currentObjectPos) - XMVectorGetY(otherObjectPos), 2) +
								pow(XMVectorGetZ(currentObjectPos) - XMVectorGetZ(otherObjectPos), 2));

		if (distanceSquared < (pow(GetBoundingSphereRadius() + otherObjectCheck->GetBoundingSphereRadius(), 2)))
		{
			return true;
		}

		
		return false; 


	}

}

float Model::GetHeight(){

	return m_height;
}

float Model::GetWidth(){

	return m_width;
}

float Model::GetDepth(){

	return m_depth;
}

void Model::AddTexture(LPCSTR filename) {

	D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice, filename, NULL, NULL, &m_pTexture, NULL);

	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	m_pD3DDevice->CreateSamplerState(&sampler_desc, &m_pSampler);

}
