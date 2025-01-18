#include "pch.h"
#include "LevelMgr.h"

#include "Level.h"
#include "GameObject.h"

#include "Transform.h"
#include "MeshRender.h"
#include "PlayerScript.h"
#include "AssetMgr.h"

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

	CGameObject* pObject = new CGameObject;
	pObject->addComponent(new CTransform);
	pObject->addComponent(new CMeshRender);
	pObject->addComponent(new CPlayerScript);

	pObject->getTransform()->setRelativeScale(0.66f, 1.0f, 1.0f);					// 이미지를 출력할 비율

	pObject->getMeshRender()->setMesh(CAssetMgr::getInstance()->FindAsset<CMesh>(L"RectMesh"));
	pObject->getMeshRender()->setShader(CAssetMgr::getInstance()->FindAsset<CGraphicShader>(L"Std2DShader"));
	pObject->getMeshRender()->setTexture(CAssetMgr::getInstance()->FindAsset<CTexture>(L"texture\\Character.png"));

	UINT nLayerIdx = 0U;
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
