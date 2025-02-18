#include "pch.h"
#include "CDbgRenderMgr.h"

#include "TimeMgr.h"
#include "AssetMgr.h"
#include "Mesh.h"
#include "Material.h"

#include "GameObject.h"
#include "Transform.h"
#include "MeshRender.h"

CDbgRenderMgr::CDbgRenderMgr()
	: m_DebugRenderObj(nullptr)
{
	m_DebugRenderObj = new CGameObject;
	m_DebugRenderObj->addComponent(new CTransform);					// 위치 계산 할 때
	m_DebugRenderObj->addComponent(new CMeshRender);				// 렌더링 할 때

	m_DebugRenderObj->MeshRender()->setMaterial(CAssetMgr::getInstance()->FindAsset<CMaterial>(L"DebugShapeMtrl"));
}

CDbgRenderMgr::~CDbgRenderMgr()
{
	if (m_DebugRenderObj)
	{
		delete m_DebugRenderObj;
		m_DebugRenderObj = nullptr;
	}
}

void CDbgRenderMgr::AddDebugShapeInfo(const tDebugShapeInfo& _info)
{
	m_lstShapeInfo.push_back(_info);
}


void CDbgRenderMgr::render()
{
	// 중간에 삭제될 수 있기 때문에 iterator를 사용한다
	std::list<tDebugShapeInfo>::iterator iter = m_lstShapeInfo.begin();

	for (; iter != m_lstShapeInfo.end();)
	{
		if (iter->matWorld == XMMatrixIdentity())
		{
			// 매트릭스가 비어있을 때 계산
			m_DebugRenderObj->Transform()->setRelativePos(iter->Position);
			m_DebugRenderObj->Transform()->setRelativeScale(iter->Scale);
			m_DebugRenderObj->Transform()->setRelativeRotation(iter->Rotation);
			m_DebugRenderObj->Transform()->finaltick();
		}
		else
		{
			m_DebugRenderObj->Transform()->setWorldMatrix(iter->matWorld);
		}

		// 메쉬
		switch (iter->Shape)
		{
		case DEBUG_SHAPE::RECT:
			m_DebugRenderObj->MeshRender()->setMesh(CAssetMgr::getInstance()->FindAsset<CMesh>(L"RectMesh_Debug"));
			break;
		case DEBUG_SHAPE::CIRCLE:
			m_DebugRenderObj->MeshRender()->setMesh(CAssetMgr::getInstance()->FindAsset<CMesh>(L"CircleMesh_Debug"));
			break;
		case DEBUG_SHAPE::LINE:
			m_DebugRenderObj->MeshRender()->setMesh(CAssetMgr::getInstance()->FindAsset<CMesh>(L"LineMesh"));
			break;
		case DEBUG_SHAPE::CUBE:
			m_DebugRenderObj->MeshRender()->setMesh(CAssetMgr::getInstance()->FindAsset<CMesh>(L"CubeMesh"));
			break;
		case DEBUG_SHAPE::SPHERE:
			m_DebugRenderObj->MeshRender()->setMesh(CAssetMgr::getInstance()->FindAsset<CMesh>(L"SphereMesh"));
			break;
		default:
			break;
		}
		
		m_DebugRenderObj->MeshRender()->getMaterial()->setScalarParam(SCALAR_PARAM::VEC4_0, iter->Color);

		// 재질 (선택해놓은 재질로 오브젝트 랜더링)
		m_DebugRenderObj->render();

		iter->Age += DT;
		if (iter->Duration <= iter->Age)
		{
			iter = m_lstShapeInfo.erase(iter);						// 삭제하면 다음것을 자동으로 대입
		}
		else {
			++iter;					// 삭제 안했으면 다음것 가리킴
		}
	}
}

void CDbgRenderMgr::calculateWorldMatrix(tDebugShapeInfo& _info)
{
	Matrix matScale = XMMatrixScaling(_info.Scale.x, _info.Scale.y, _info.Scale.z);

	Matrix matRotation = XMMatrixRotationX(_info.Rotation.x);
	matRotation *= XMMatrixRotationY(_info.Rotation.y);
	matRotation *= XMMatrixRotationZ(_info.Rotation.z);

	Matrix matTranslation = XMMatrixTranslation(_info.Position.x, _info.Position.y, _info.Position.z);

	_info.matWorld = matScale * matRotation * matTranslation;
}
