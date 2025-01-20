#include "pch.h"
#include "Material.h"
#include "Device.h"
#include "ConstBuffer.h"
#include "GraphicShader.h"

CMaterial::CMaterial()
	: CAsset(ASSET_TYPE::MATERIAL)
{
	//m_const.iArr[0] = 2;					// �б�ó���� �����ϸ� ������ ���������� ����� �ǰ� ȿ���� �� �� ����
}

CMaterial::~CMaterial()
{

}

void CMaterial::binding()
{
	if (m_shader.Get())
	{
		m_shader->binding();
	}

	CConstBuffer* pMtrlCB = CDevice::getInstance()->getConstBuffer(CB_TYPE::MATERIAL);
	pMtrlCB->setData(&m_const);
	pMtrlCB->binding();
}
