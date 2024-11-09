#include "TriangleDX11.h"

#include <DirectXMath.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

// Vertex struct
struct ST_Vertex 
{
	DirectX::XMFLOAT3 pos;
};

// 생성자
CTriangleDX11::CTriangleDX11()
{
	m_pVSGreenColor = NULL;
	m_pPSGreenColor = NULL;
	m_pInputLayout = NULL;
	m_pVertexBuffer = NULL;
}

CTriangleDX11::~CTriangleDX11()
{
}

bool CTriangleDX11::LoadContent()
{
	// vertex 셰이더 컴파일
	ID3DBlob* pVSBuffer = NULL;
	bool res = CompileShader(L"ShaderGreenColor.fx", "VS_Main", "vs_4_0", &pVSBuffer);
	if (res == false) {
		::MessageBox(m_hWnd, L"Unable to load vertex shader", L"ERROR", MB_OK);
		return false;
	}

	// Create vertex shader
	HRESULT hr;
	hr = m_pD3DDevice->CreateVertexShader(
		pVSBuffer->GetBufferPointer(),
		pVSBuffer->GetBufferSize(),
		0, &m_pVSGreenColor
	);

	if (FAILED(hr)) {
		if (pVSBuffer) {
			pVSBuffer->Release();
		}
		return false;
	}

	// Define input layout
	D3D11_INPUT_ELEMENT_DESC shaderInputLayout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	unsigned int numLayoutElements = ARRAYSIZE(shaderInputLayout);

	hr = m_pD3DDevice->CreateInputLayout(
		shaderInputLayout, numLayoutElements,
		pVSBuffer->GetBufferPointer(),
		pVSBuffer->GetBufferSize(),
		&m_pInputLayout
	);
	if (FAILED(hr)) {
		return false;
	}

	// Release VS Buffer
	pVSBuffer->Release();
	pVSBuffer = NULL;

	ID3DBlob* pPSBuffer = NULL;
	res = CompileShader(L"ShaderGreenColor.fx", "PS_Main", "ps_4_0", &pPSBuffer);
	if (res == false) {
		::MessageBox(m_hWnd, L"Unable to load pixel shader", L"ERROR", MB_OK);
		return false;
	}

	// Create pixel shader
	hr = m_pD3DDevice->CreatePixelShader(
		pPSBuffer->GetBufferPointer(),
		pPSBuffer->GetBufferSize(),
		0, &m_pPSGreenColor
	);
	if (FAILED(hr)) {
		return false;
	}

	// Cleanup PS buffer
	pPSBuffer->Release();
	pPSBuffer = NULL;

	// Define Triangle
	ST_Vertex vertices[] = {
		DirectX::XMFLOAT3(0.0f,  0.5f, 0.5f),
		DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f),
		DirectX::XMFLOAT3(-0.5f, -0.5f, 0.5f)
	};


	// vertex description
	D3D11_BUFFER_DESC vertexDesc;
	memset(&vertexDesc, 0, sizeof(vertexDesc));
	vertexDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.ByteWidth = sizeof(ST_Vertex) * 3;

	// 리소스 데이터
	D3D11_SUBRESOURCE_DATA resourceData;
	memset(&resourceData, 0, sizeof(resourceData));
	resourceData.pSysMem = vertices;

	// vertex 버퍼 생성
	hr = m_pD3DDevice->CreateBuffer(&vertexDesc, &resourceData, &m_pVertexBuffer);
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

void CTriangleDX11::UnloadContent()
{
	if (m_pVSGreenColor) {
		m_pVSGreenColor->Release();
		m_pVSGreenColor = NULL;
	}
	if (m_pPSGreenColor) {
		m_pPSGreenColor->Release();
		m_pPSGreenColor = NULL;
	}
	if (m_pInputLayout) {
		m_pInputLayout->Release();
		m_pInputLayout = NULL;
	}
	if (m_pVertexBuffer) {
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}
}

void CTriangleDX11::Update()
{
}

void CTriangleDX11::Render()
{
	if (m_pD3DContext == NULL) {
		return;
	}

	// 배경색으로 지운다
	float color[4] = COLOR_RGBA(71, 78, 114, 255);
	m_pD3DContext->ClearRenderTargetView(m_pD3DRenderTargetView, color);

	

	// 버퍼 교체
	m_pSwapChain->Present(0, 0);
}
