#include "pch.h"
#include "AssetMgr.h"
#include "PathMgr.h"

void CAssetMgr::init()
{
	createDefaultMesh();
	createDefaultTexture();
	createDefaultMaterial();
	createDefaultGraphicShader();
	createDefaultComputeShader();
}

void CAssetMgr::createDefaultMesh()
{
	Ptr<CMesh> pMesh = nullptr;

	std::vector<Vtx> vecVtx;
	std::vector<UINT> vecIdx;
	Vtx v;

	// =========
	// Rect Mesh
	// =========
	// 0 -- 1
	// | \  |
	// 3 -- 2
	v.vPos = Vec3(-0.5, 0.5f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, 0.5f, 0.f);
	v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, -0.5f, 0.f);
	v.vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(-0.5, -0.5f, 0.f);
	v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	vecVtx.push_back(v);

	// Index Buffer
	vecIdx.push_back(0);
	vecIdx.push_back(2);
	vecIdx.push_back(3);

	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(2);

	pMesh = new CMesh;
	pMesh->create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddAsset<CMesh>(L"RectMesh", pMesh);

	vecVtx.clear();
	vecIdx.clear();

	// ===========
	// Circle Mesh
	// ===========
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	vecVtx.push_back(v);

	float Radius = 0.5f;
	UINT Slice = 60;
	float AngleStep = (2 * XM_PI) / Slice;

	float Angle = 0.f;
	for (UINT i = 0; i <= Slice; ++i)
	{
		v.vPos = Vec3(cosf(Angle) * Radius, sinf(Angle) * Radius, 0.f);
		v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
		vecVtx.push_back(v);

		Angle += AngleStep;
	}

	for (UINT i = 0; i < Slice; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	pMesh = new CMesh;
	pMesh->create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddAsset<CMesh>(L"CircleMesh", pMesh);
	vecVtx.clear();
	vecIdx.clear();
}

void CAssetMgr::createDefaultTexture()
{
	// 텍스처 로딩 (키값, 경로값)
	Ptr<CTexture> pTex = load<CTexture>(L"texture\\Character.png", L"texture\\Character.png");
}

void CAssetMgr::createDefaultMaterial()
{
}

void CAssetMgr::createDefaultGraphicShader()
{
	Ptr<CGraphicShader> pShader = nullptr;
	std::wstring strPath = std::wstring(CPathMgr::getInstance()->getResPath());

	pShader = new CGraphicShader;
	pShader->createVertexShader(strPath + L"shader\\std2d.fx", "VS_Std2D");
	pShader->createPixelShader(strPath + L"shader\\std2d.fx", "PS_Std2D");

	// 등록한다
	AddAsset<CGraphicShader>(L"Std2DShader", pShader);
}

void CAssetMgr::createDefaultComputeShader()
{
}