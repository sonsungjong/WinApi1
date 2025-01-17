#include "pch.h"
#include "MeshRender.h"

#include "Transform.h"

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
	// ������Ʈ�� ��ġ���� ������۸� ���ؼ� ���ε�
	getOwner()->getTransform()->binding();

	// ����� ���̴� ���ε�
	getShader()->binding();

	// �޽� ���ε� �� ������
	getMesh()->render();
}
