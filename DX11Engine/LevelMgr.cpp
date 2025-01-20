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

	m_curLevel->addObject(nLayerIdx, pObject);
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
