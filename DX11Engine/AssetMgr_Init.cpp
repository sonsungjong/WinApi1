#include "pch.h"
#include "AssetMgr.h"
#include "PathMgr.h"

void CAssetMgr::init()
{
	createDefaultMesh();
	createDefaultTexture();
	createDefaultGraphicShader();
	createDefaultComputeShader();

	createDefaultMaterial();
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
	v.vUV = Vec2(0.f, 0.f);
	vecVtx.push_back(v);				// 0��° ��ǥ ����

	v.vPos = Vec3(0.5f, 0.5f, 0.f);
	v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	v.vUV = Vec2(1.f, 0.f);
	vecVtx.push_back(v);				// 1��° ��ǥ ����

	v.vPos = Vec3(0.5f, -0.5f, 0.f);
	v.vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	v.vUV = Vec2(1.f, 1.f);
	vecVtx.push_back(v);				// 2��° ��ǥ ����

	v.vPos = Vec3(-0.5, -0.5f, 0.f);
	v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	v.vUV = Vec2(0.f, 1.f);
	vecVtx.push_back(v);				// 3��° ��ǥ ����

	// Index Buffer (CULL_BACK(�ݽð��������) �ɼ��϶��� �ð��������� push_back �� ��, CULL_FRONT(�ð��������) �ɼ��϶��� �ݽð��������� push_back �� ��)
	vecIdx.push_back(0);
	vecIdx.push_back(2);
	vecIdx.push_back(3);

	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(2);

	pMesh = new CMesh;
	pMesh->create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddAsset<CMesh>(L"RectMesh", pMesh);

	vecIdx.clear();
	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(2);
	vecIdx.push_back(3);
	vecIdx.push_back(0);

	pMesh = new CMesh;
	pMesh->create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddAsset<CMesh>(L"RectMesh_Debug", pMesh);

	vecVtx.clear();
	vecIdx.clear();

	// ===========
	// Circle Mesh
	// ===========
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	vecVtx.push_back(v);

	float Radius = 0.5f;				// �⺻ ������
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

	vecIdx.clear();
	for (int i = 0; i <= Slice; ++i)
	{
		vecIdx.push_back(i + 1);
	}

	pMesh = new CMesh;
	pMesh->create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddAsset<CMesh>(L"CircleMesh_Debug", pMesh);

	vecVtx.clear();
	vecIdx.clear();
}

void CAssetMgr::createDefaultTexture()
{
	// �ؽ�ó �ε� (Ű��, ��ΰ�)
	load<CTexture>(L"texture\\Character.png", L"texture\\Character.png");
	load<CTexture>(L"texture\\Fighter.bmp", L"texture\\Fighter.bmp");
}

void CAssetMgr::createDefaultMaterial()
{
	Ptr<CMaterial> pMaterial = nullptr;

	// Std2D Material
	pMaterial = new CMaterial;
	pMaterial->setName(L"Std2DMtrl");
	pMaterial->setShader(FindAsset<CGraphicShader>(L"Std2DShader"));
	AddAsset<CMaterial>(pMaterial->getName(), pMaterial);

	// DebugShape Material
	pMaterial = new CMaterial;
	pMaterial->setName(L"DebugShapeMtrl");
	pMaterial->setShader(FindAsset<CGraphicShader>(L"DebugShapeShader"));
	AddAsset<CMaterial>(pMaterial->getName(), pMaterial);
}

void CAssetMgr::createDefaultGraphicShader()
{
	std::wstring strPath = std::wstring(CPathMgr::getInstance()->getResPath());
	Ptr<CGraphicShader> pShader = nullptr;

	/*
		Std2DShader
	*/
	pShader = new CGraphicShader;
	pShader->createVertexShader(strPath + L"shader\\std2d.fx", "VS_Std2D");
	pShader->createPixelShader(strPath + L"shader\\std2d.fx", "PS_Std2D");
	pShader->setRasterizerStateType(RS_TYPE::CULL_NONE);			// �ø��� ���� �ʴ´� (�ݴ��� ������)
	pShader->setBlendStateType(BS_TYPE::ALPHA_BLEND);					// ���İ� �ݿ�
	pShader->setDepthStencilStateType(DS_TYPE::LESS);
	AddAsset<CGraphicShader>(L"Std2DShader", pShader);				// ����Ѵ�

	/*
		DebugShapeShader
	*/
	pShader = new CGraphicShader;
	pShader->createVertexShader(strPath + L"shader\\debug_shape.fx", "VS_DebugShape");
	pShader->createPixelShader(strPath + L"shader\\debug_shape.fx", "PS_DebugShape");
	pShader->setRasterizerStateType(RS_TYPE::CULL_NONE);				// �ø��� ���� �ʴ´� (�ݴ��� ������)
	pShader->setBlendStateType(BS_TYPE::DEFAULT);
	pShader->setDepthStencilStateType(DS_TYPE::NO_TEST_NO_WRITE);				// ����׿� ������ ���� �׽�Ʈ�� ���ϰ� ���� ��ϵ� ���ϰ�
	pShader->setTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);						// ����
	AddAsset<CGraphicShader>(L"DebugShapeShader", pShader);				// ����Ѵ�
}

void CAssetMgr::createDefaultComputeShader()
{
}