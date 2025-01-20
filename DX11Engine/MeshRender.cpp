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
	// ������Ʈ�� ��ġ���� ������۸� ���ؼ� ���ε�
	getOwner()->getTransform()->binding();

	// ����� ���̴� ���ε�
	getMaterial()->binding();

	// ����� �ؽ��� ���ε�
	//getMaterial()->getTexture()->binding(0);			// t0 �� ���ε�

	// �޽� ���ε� �� ������
	getMesh()->render();
}
