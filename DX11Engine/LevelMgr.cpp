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

	// Camera Object 생성
	CGameObject* pCamObj = new CGameObject;
	pCamObj->setName(L"MainCamera");
	pCamObj->addComponent(new CTransform);
	pCamObj->addComponent(new CCamera);
	pCamObj->addComponent(new CCameraMoveScript);

	pCamObj->Camera()->setCameraPriority(nLayerIdx);				// 메인카메라(0)
	pCamObj->Camera()->setProjType(PROJ_TYPE::ORTHOGRAPHIC);				// 직교 투영 방식으로 렌더링

	m_curLevel->addObject(nLayerIdx, pCamObj);

	// Player Object 생성
	CGameObject* pObject = new CGameObject;
	pObject->setName(L"Player");
	pObject->addComponent(new CTransform);					// 이동 관련
	pObject->addComponent(new CMeshRender);				// 랜더링 해줌
	pObject->addComponent(new CCollider2D);					// 충돌
	pObject->addComponent(new CPlayerScript);				// 플레이어 기능

	pObject->Transform()->setRelativePos(Vec3(0.f, 0.f, 100.f));
	pObject->Transform()->setRelativeScale(100.f, 100.f, 0.5f);					// 이미지를 출력할 비율

	pObject->MeshRender()->setMesh(CAssetMgr::getInstance()->FindAsset<CMesh>(L"RectMesh"));
	pObject->MeshRender()->setMaterial(CAssetMgr::getInstance()->FindAsset<CMaterial>(L"Std2DMtrl"));
	pObject->MeshRender()->getMaterial()->setScalarParam(SCALAR_PARAM::INT_0, 0);				// INT_0 자리에 0를 전달해라 (fx파일의 0번 재질 선택)
	pObject->MeshRender()->getMaterial()->setTexParam(TEX_PARAM::TEX_0, CAssetMgr::getInstance()->FindAsset<CTexture>(L"texture\\Fighter.bmp"));

	pObject->Collider2D()->setAbsolute(false);								// true : 이미지 비율에 영향을 받지 않게함, false : 오브젝트 크기에 의존
	pObject->Collider2D()->setOffset(Vec3(0.f, 0.f, 0.f));				// 이미지 비율에 영향을 받음 (Absolute를 키면 절대값으로 비율없이 바로 적용)
	pObject->Collider2D()->setScale(Vec3(0.2f, 0.6f, 1.f));			// 스케일 1.5배로 변경

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
