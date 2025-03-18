#include "pch.h"
#include "LevelMgr.h"

#include "AssetMgr.h"

#include "Level.h"
#include "Layer.h"
#include "GameObject.h"
#include "components.h"

#include "PlayerScript.h"
#include "CameraMoveScript.h"

#include "CollisionMgr.h"

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
	m_curLevel->getLayer(0)->setName(L"Default");
	m_curLevel->getLayer(1)->setName(L"Player");
	m_curLevel->getLayer(2)->setName(L"Monster");
	unsigned int main_camera_layer = 0U;
	unsigned int main_player_layer = 1U;
	unsigned int main_monster_layer = 2U;

	// Camera Object ����
	CGameObject* pCamObj = new CGameObject;
	pCamObj->setName(L"MainCamera");
	pCamObj->addComponent(new CTransform);
	pCamObj->addComponent(new CCamera);
	pCamObj->addComponent(new CCameraMoveScript);

	pCamObj->Camera()->setCameraPriority(main_camera_layer);				// ����ī�޶�(0)
	pCamObj->Camera()->setProjType(PROJ_TYPE::ORTHOGRAPHIC);				// ���� ���� ������� ������

	m_curLevel->addObject(main_camera_layer, pCamObj);

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
	pObject->MeshRender()->getMaterial()->setTexParam(TEX_PARAM::TEX_0, CAssetMgr::getInstance()->FindAsset<CTexture>(L"texture\\Character.png"));

	pObject->Collider2D()->setAbsolute(false);								// true : �̹��� ������ ������ ���� �ʰ���, false : ������Ʈ ũ�⿡ ����
	pObject->Collider2D()->setOffset(Vec3(0.f, 0.f, 0.f));				// �̹��� ������ ������ ���� (Absolute�� Ű�� ���밪���� �������� �ٷ� ����)
	pObject->Collider2D()->setScale(Vec3(0.2f, 0.6f, 1.f));			// ������ 1.5��� ����

	m_curLevel->addObject(main_player_layer, pObject);

	// Monster Object ����
	pObject = new CGameObject;
	pObject->setName(L"Monster");
	pObject->addComponent(new CTransform);					// �̵� ����
	pObject->addComponent(new CMeshRender);				// ������ ����
	pObject->addComponent(new CCollider2D);					// �浹

	pObject->Transform()->setRelativePos(Vec3(50.f, 0.f, 300.f));
	pObject->Transform()->setRelativeScale(200.f, 200.f, 0.5f);					// �̹����� ����� ����

	pObject->MeshRender()->setMesh(CAssetMgr::getInstance()->FindAsset<CMesh>(L"RectMesh"));
	pObject->MeshRender()->setMaterial(CAssetMgr::getInstance()->FindAsset<CMaterial>(L"Std2DMtrl"));

	pObject->Collider2D()->setAbsolute(false);								// true : �̹��� ������ ������ ���� �ʰ���, false : ������Ʈ ũ�⿡ ����
	pObject->Collider2D()->setOffset(Vec3(0.f, 0.f, 0.f));				// �̹��� ������ ������ ���� (Absolute�� Ű�� ���밪���� �������� �ٷ� ����)
	pObject->Collider2D()->setScale(Vec3(0.2f, 0.6f, 1.f));			// ������ 1.5��� ����

	m_curLevel->addObject(main_monster_layer, pObject);

	// Level �浹 ����
	CCollisionMgr::getInstance()->LayerCheck(2, 1);				// 1�� ���̾�� 2�� ���̾� �浹 �����ϰ�
	CCollisionMgr::getInstance()->LayerCheck(17, 22);				
	CCollisionMgr::getInstance()->LayerCheck(28, 2);				
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
