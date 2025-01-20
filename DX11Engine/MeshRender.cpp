#include "pch.h"
#include "MeshRender.h"

#include "Transform.h"
#include "Material.h"

CMeshRender::CMeshRender()
	: CRenderComponent(COMPONENT_TYPE::MESHRENDER)
{
}

CMeshRender::~CMeshRender()
{
}

void CMeshRender::finaltick()
{
}

void CMeshRender::render()
{
	// 오브젝트의 위치값을 상수버퍼를 통해서 바인딩
	getOwner()->getTransform()->binding();

	// 사용할 쉐이더 바인딩
	getMaterial()->binding();

	// 사용할 텍스쳐 바인딩
	//getMaterial()->getTexture()->binding(0);			// t0 에 바인딩

	// 메시 바인딩 및 렌더링
	getMesh()->render();
}
