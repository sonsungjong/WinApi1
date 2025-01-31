#include "pch.h"
#include "LevelMgr.h"

#include "AssetMgr.h"

#include "Level.h"
#include "GameObject.h"
#include "components.h"

#include "PlayerScript.h"
#include "CameraMoveScript.h"

CLevelMgr::CLevelMgr()
	: m_curLevel(nullptr)
{
}

CLevelMgr::~CLevelMgr()
{
	if (m_curLevel) {
		delete m_curLevel;
		m_curLevel = nullptr;
	}
}

void CLevelMgr::init()
{
	m_curLevel = new CLevel;
	UINT nLayerIdx = 0U;

	// Camera Object ����
	CGameObject* pCamObj = new CGameObject;
	pCamObj->setName(L"MainCamera");
	pCamObj->addComponent(new CTransform);
	pCamObj->addComponent(new CCamera);
	pCamObj->addComponent(new CCameraMoveScript);

	pCamObj->getCamera()->setCameraPriority(0);				// ����ī�޶�(0)

	m_curLevel->addObject(nLayerIdx, pCamObj);

	// Player Object ����
	CGameObject* pObject = new CGameObject;
	pObject->setName(L"Player");
	pObject->addComponent(new CTransform);					// �̵� ����
	pObject->addComponent(new CMeshRender);				// ������ ����
	pObject->addComponent(new CPlayerScript);				// �÷��̾� ���

	pObject->getTransform()->setRelativePos(Vec3(0.f, 0.f, 100.f));
	pObject->getTransform()->setRelativeScale(100.f, 100.f, 0.5f);					// �̹����� ����� ����

	pObject->getMeshRender()->setMesh(CAssetMgr::getInstance()->FindAsset<CMesh>(L"RectMesh"));
	pObject->getMeshRender()->setMaterial(CAssetMgr::getInstance()->FindAsset<CMaterial>(L"Std2DMtrl"));
	pObject->getMeshRender()->getMaterial()->setScalarParam(SCALAR_PARAM::INT_0, 1);				// INT_0 �ڸ��� 1�� �����ض� (fx������ 0�� ���� ����)
	//pObject->getMeshRender()->getMaterial()->setScalarParam(SCALAR_PARAM::VEC4_3, Vec4(0.f, 3.14f, 0.f, 0.f));
	Matrix mat;
	mat._11 = 2.1f;			// 1��1�� (�ε����δ� 0,0 )
	pObject->getMeshRender()->getMaterial()->setScalarParam(SCALAR_PARAM::MAT_1, mat);
	pObject->getMeshRender()->getMaterial()->setTexParam(TEX_PARAM::TEX_0, CAssetMgr::getInstance()->FindAsset<CTexture>(L"texture\\Fighter.bmp"));

	m_curLevel->addObject(nLayerIdx, pObject);

	DrawDebugRect(Vec3(0.f, 0.f, 500.f), Vec3(100.f, 100.f, 1.f), Vec3(0.f, 0.f, 0.f), Vec4(0.f, 1.f, 0.f, 1.f), 5.f);
}

void CLevelMgr::tick()
{
	if (m_curLevel) {
		m_curLevel->tick();
		m_curLevel->finaltick();
	}
}

void CLevelMgr::render()
{
	if (m_curLevel) {
		m_curLevel->render();
	}
}
