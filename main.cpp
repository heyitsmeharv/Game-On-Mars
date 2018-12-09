//////////////////////////////////////////////////////////////////////////////////////
//							HEADER FILES
//////////////////////////////////////////////////////////////////////////////////////
#define _XM_NO_INTRINSICS
#define XM_NO_ALIGNMENT
#include "Input.h"
#include "Game.h"
#include "HUD.h"
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <xnamath.h>

//////////////////////////////////////////////////////////////////////////////////////
//							Global Variables
//////////////////////////////////////////////////////////////////////////////////////
HINSTANCE					g_hInst = NULL;
HWND						g_hWnd = NULL;
D3D_DRIVER_TYPE				g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL			g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*				g_pD3DDevice = NULL;
ID3D11DeviceContext*		g_pImmediateContext = NULL;
IDXGISwapChain*				g_pSwapChain = NULL;
ID3D11RenderTargetView*		g_pBackBufferRTView = NULL;
ID3D11ShaderResourceView*	g_pTexture0;
ID3D11SamplerState*			g_pSampler0;
ID3D11DepthStencilView*		g_pZBuffer;

//////////////////////////////////////////////////////////////////////////////////////
//							Input Variables
//////////////////////////////////////////////////////////////////////////////////////
IDirectInput8*				g_direct_input;
IDirectInputDevice8*		g_keyboard_device;
unsigned char				g_keyboard_keys_state[256];


//////////////////////////////////////////////////////////////////////////////////////
//							Vertex and Pixel Shader
//////////////////////////////////////////////////////////////////////////////////////
ID3D11Buffer*				g_pVertexBuffer;
ID3D11VertexShader*			g_pVertexShader;
ID3D11PixelShader*			g_pPixelShader;
ID3D11InputLayout*			g_pInputLayout;

//Game*						m_pGame;			// CALL GAME'S UPDATE

//////////////////////////////////////////////////////////////////////////////////////
//	NAME
//////////////////////////////////////////////////////////////////////////////////////
char		g_TutorialName[100] = "Adam Harvey Game On Mars\0";



//////////////////////////////////////////////////////////////////////////////////////
//							Lighting Varibales
//////////////////////////////////////////////////////////////////////////////////////
XMVECTOR					g_directional_light_shines_from;
XMVECTOR					g_directional_light_colour;
XMVECTOR					g_ambient_light_colour;


//////////////////////////////////////////////////////////////////////////////////////
//							Forward declarations
//////////////////////////////////////////////////////////////////////////////////////
HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT InitialiseD3D();
HRESULT InitialiseGraphics(void);
HRESULT InitialiseInput();
void ShutdownD3D();
void RenderFrame();

//////////////////////////////////////////////////////////////////////////////////////
//						Clean up D3D objects
//////////////////////////////////////////////////////////////////////////////////////
void ShutdownD3D()
{
	//if (g_Model) delete g_Model;
	//if (g_pConstantBuffer0) g_pConstantBuffer0->Release();
	if (g_pVertexBuffer) g_pVertexBuffer->Release();
	if (g_pInputLayout) g_pInputLayout->Release();
	if (g_pVertexShader) g_pVertexShader->Release();
	if (g_pVertexBuffer) g_pVertexBuffer->Release();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pTexture0) g_pTexture0->Release();
	if (g_pSampler0) g_pSampler0->Release();
	if (g_pImmediateContext) g_pImmediateContext->Release();
	if (g_pD3DDevice) g_pD3DDevice->Release();
	if (g_pBackBufferRTView) g_pBackBufferRTView->Release();

}

//////////////////////////////////////////////////////////////////////////////////////
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (FAILED(InitialiseWindow(hInstance, nCmdShow)))
	{
		DXTRACE_MSG("Failed to create Window");
		return 0;
	}

	if (FAILED(InitialiseD3D()))
	{
		DXTRACE_MSG("Failed to create Device");
		ShutdownD3D();
		return 0;
	}

	if (FAILED(InitialiseInput())) {
		DXTRACE_MSG("Failed to initialise Input");
		return 0;
	}

	if (FAILED(TheInput::Singleton()->InitialiseInput(&g_hInst, &g_hWnd))) {
		DXTRACE_MSG("Failed to initialise Input");
		return 0;
	}

	if (!TheGame::Singleton()->Init(g_pD3DDevice, g_pImmediateContext)) {
		DXTRACE_MSG("Failed to initialise Game");
		return 0;
	}

	
	///////////////////////////////////////////
	// Main message loop
	///////////////////////////////////////////
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			///////////////////////////////////////////
			// do something
			///////////////////////////////////////////
			RenderFrame(); 

		}
	}

	return (int)msg.wParam;
}

//////////////////////////////////////////////////////////////////////////////////////
//						Register class and create window
//////////////////////////////////////////////////////////////////////////////////////
HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow)
{
	///////////////////////////////////////////
	// Give your app window your own name
	///////////////////////////////////////////
	char Name[100] = "Adam Harvey\0";

	///////////////////////////////////////////
	// Register class
	///////////////////////////////////////////
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	//   wcex.hbrBackground = (HBRUSH )( COLOR_WINDOW + 1); // Needed for non-D3D apps
	wcex.lpszClassName = Name;

	if (!RegisterClassEx(&wcex)) return E_FAIL;

	///////////////////////////////////////////
	// Create window
	///////////////////////////////////////////
	g_hInst = hInstance;
	RECT rc = { 0, 0, 1080, 720 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(Name, g_TutorialName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left,
		rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
	if (!g_hWnd)
		return E_FAIL;

	ShowWindow(g_hWnd, nCmdShow);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////////////
//						 Create D3D device and swap chain
//////////////////////////////////////////////////////////////////////////////////////
HRESULT InitialiseD3D()
{
	HRESULT hr = S_OK;
	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;
	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG; // Nope.
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE, // comment out this line if you need to test D3D 11.0 functionality on hardware that doesn't support it
		D3D_DRIVER_TYPE_WARP, // comment this out also to use reference device
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL,
			createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &g_pSwapChain,
			&g_pD3DDevice, &g_featureLevel, &g_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
		return hr;

	///////////////////////////////////////////
	// Get pointer to back buffer texture
	///////////////////////////////////////////
	ID3D11Texture2D *pBackBufferTexture;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		(LPVOID*)&pBackBufferTexture);

	if (FAILED(hr)) return hr;

	/////////////////////////////////////////////////////////////////////////
	// Use the back buffer texture pointer to create the render target view
	/////////////////////////////////////////////////////////////////////////
	hr = g_pD3DDevice->CreateRenderTargetView(pBackBufferTexture, NULL,
		&g_pBackBufferRTView);
	pBackBufferTexture->Release();

	if (FAILED(hr)) return hr;

	///////////////////////////////////////////
	// Create a Z buffer texture
	///////////////////////////////////////////
	D3D11_TEXTURE2D_DESC tex2dDesc;
	ZeroMemory(&tex2dDesc, sizeof(tex2dDesc));

	tex2dDesc.Width = width;
	tex2dDesc.Height = height;
	tex2dDesc.ArraySize = 1;
	tex2dDesc.MipLevels = 1;
	tex2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tex2dDesc.SampleDesc.Count = sd.SampleDesc.Count;
	tex2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tex2dDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D *pZBufferTexture;
	hr = g_pD3DDevice->CreateTexture2D(&tex2dDesc, NULL, &pZBufferTexture);

	if (FAILED(hr)) return hr;

	///////////////////////////////////////////
	// Create the Z buffer
	///////////////////////////////////////////
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));

	dsvDesc.Format = tex2dDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	g_pD3DDevice->CreateDepthStencilView(pZBufferTexture, &dsvDesc, &g_pZBuffer);
	pZBufferTexture->Release();

	///////////////////////////////////////////
	// Set the render target view
	///////////////////////////////////////////
	g_pImmediateContext->OMSetRenderTargets(1, &g_pBackBufferRTView, g_pZBuffer);

	///////////////////////////////////////////
	// Set the viewport
	///////////////////////////////////////////
	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	g_pImmediateContext->RSSetViewports(1, &viewport);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////////////
//				Called every time the application receives a message
//////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}


	return 0;
}


HRESULT InitialiseInput() {

	HRESULT hr;
	ZeroMemory(g_keyboard_keys_state, sizeof(g_keyboard_keys_state));

	hr = DirectInput8Create(g_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_direct_input, NULL);
	if (FAILED(hr)) return hr;

	hr = g_direct_input->CreateDevice(GUID_SysKeyboard, &g_keyboard_device, NULL);
	if (FAILED(hr)) return hr;

	hr = g_keyboard_device->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr)) return hr;

	hr = g_keyboard_device->SetCooperativeLevel(g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) return hr;

	hr = g_keyboard_device->Acquire();
	if (FAILED(hr)) return hr;


	return S_OK;

}

void RenderFrame(){

	//////////////////////////////////////////////////////
	// Clear the back buffer - choose a colour you like
	//////////////////////////////////////////////////////
	float rgba_clear_colour[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // Black
	g_pImmediateContext->ClearRenderTargetView(g_pBackBufferRTView, rgba_clear_colour);

	g_pImmediateContext->ClearDepthStencilView
		(g_pZBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	TheGame::Singleton()->update();


	g_pSwapChain->Present(0, 0);

}

