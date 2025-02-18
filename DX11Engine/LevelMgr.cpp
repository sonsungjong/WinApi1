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

	pCamObj->Camera()->setCameraPriority(nLayerIdx);				// ����ī�޶�(0)
	pCamObj->Camera()->setProjType(PROJ_TYPE::ORTHOGRAPHIC);				// ���� ���� ������� ������

	m_curLevel->addObject(nLayerIdx, pCamObj);

	// Player Object ����
	CGameObject* pObject = new CGameObject;
	pObject->setName(L"Player");
	pObject->addComponent(new CTransform);					// �̵� ����
	pObject->addComponent(new CMeshRender);				// ������ ����
	pObject->addComponent(new CCollider2D);					// �浹
	pObject->addComponent(new CPlayerScript);				// �÷��̾� ���

	pObject->Transform()->setRelativePos(Vec3(0.f, 0.f, 100.f));
	pObject->Transform()->setRelativeScale(100.f, 100.f, 0.5f);					// �̹����� ����� ����

	pObject->MeshRender()->setMesh(CAssetMgr::getInstance()->FindAsset<CMesh>(L"RectMesh"));
	pObject->MeshRender()->setMaterial(CAssetMgr::getInstance()->FindAsset<CMaterial>(L"Std2DMtrl"));
	pObject->MeshRender()->getMaterial()->setScalarParam(SCALAR_PARAM::INT_0, 0);				// INT_0 �ڸ��� 0�� �����ض� (fx������ 0�� ���� ����)
	pObject->MeshRender()->getMaterial()->setTexParam(TEX_PARAM::TEX_0, CAssetMgr::getInstance()->FindAsset<CTexture>(L"texture\\Fighter.bmp"));

	pObject->Collider2D()->setAbsolute(false);								// true : �̹��� ������ ������ ���� �ʰ���, false : ������Ʈ ũ�⿡ ����
	pObject->Collider2D()->setOffset(Vec3(0.f, 0.f, 0.f));				// �̹��� ������ ������ ���� (Absolute�� Ű�� ���밪���� �������� �ٷ� ����)
	pObject->Collider2D()->setScale(Vec3(0.2f, 0.6f, 1.f));			// ������ 1.5��� ����

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
