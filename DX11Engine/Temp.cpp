#include "pch.h"
#include "Temp.h"
#include "Device.h"

// 정점 정보 저장 버퍼
ComPtr<ID3D11Buffer> g_VB;
ComPtr<ID3D11InputLayout> g_layout;					// 정점 하나를 구성하는 Layout 정보

// 포지션과 컬러 3개
Vtx g_arrVtx[3] = {};

// HLSL (어셈블리가 아니라 C++과 유사하게 쉐이더 코드를 컴파일해주는 형식) [.fx파일 속성에서.. 셰이더형식 /fx, Shader Model 5.0 설정]

// Vertex Shader (정점당)
ComPtr<ID3DBlob> g_VSBlob;					// 컴파일한 쉐이더 코드를 저장
ComPtr<ID3D11VertexShader> g_VS;

// Pixel Shader (픽셀당)
ComPtr<ID3DBlob> g_PSBlob;
ComPtr<ID3D11PixelShader> g_PS;


int TempInit()
{
	// 모니터를 -1 ~ 1 까지 중앙을 0,0으로 정규화한다 [NDC좌표]
	g_arrVtx[0].vPos = Vec3(0.f, 1.f, 0.f);
	g_arrVtx[0].vColor = Vec4(0.f, 1.f, 0.f, 1.f);			// 정규화해서 255를 0.0f~1.0f 로 (RGBA)

	g_arrVtx[1].vPos = Vec3(1.f, -1.f, 0.f);
	g_arrVtx[1].vColor = Vec4(0.f, 1.f, 0.f, 1.f);			// 정규화해서 255를 0.0f~1.0f 로

	g_arrVtx[2].vPos = Vec3(-1.f, -1.f, 0.f);
	g_arrVtx[2].vColor = Vec4(0.f, 0.f, 1.f, 1.f);			// 정규화해서 255를 0.0f~1.0f 로

	// 정점 버퍼 생성
	D3D11_BUFFER_DESC VBDesc = {};

	VBDesc.ByteWidth = sizeof(Vtx) * 3;
	VBDesc.MiscFlags = 0;

	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;						// 용도

	// 덮어쓰기 가능 설정
	VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VBDesc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA SubDesc = {};
	SubDesc.pSysMem = g_arrVtx;
	if (FAILED(DEVICE->CreateBuffer(&VBDesc, &SubDesc, g_VB.GetAddressOf()))) {
		return E_FAIL;
	}
	// 버텍스 쉐이더

	// 정점 구성 Layout 정보 설정
	D3D11_INPUT_ELEMENT_DESC LayoutDesc[2] = {};				// 위치와 색상
	unsigned int nOffset = 0U;

	LayoutDesc[0].AlignedByteOffset = nOffset;
	LayoutDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;					// VEC3 (12byte, float3)
	LayoutDesc[0].InputSlot = 0;
	LayoutDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	LayoutDesc[0].InstanceDataStepRate = 0;
	LayoutDesc[0].SemanticName = "POSITION";
	LayoutDesc[0].SemanticIndex = 0;															// POSITION0
	nOffset += 12U;
	
	LayoutDesc[1].AlignedByteOffset = nOffset;
	LayoutDesc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;				// VEC4 (16byte, float4)
	LayoutDesc[1].InputSlot = 0;
	LayoutDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	LayoutDesc[1].InstanceDataStepRate = 0;
	LayoutDesc[1].SemanticName = "COLOR";
	LayoutDesc[1].SemanticIndex = 0;															// COLOR0
	nOffset += 16U;

	// DEVICE->CreateInputLayout(LayoutDesc, 2, );

	// 픽셀 셰이더

	return S_OK;
}

void TempRelease()
{

}

void TempTick()
{

}

void TempRender()
{

}
