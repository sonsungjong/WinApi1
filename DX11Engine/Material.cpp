#include "pch.h"
#include "Material.h"
#include "Device.h"
#include "ConstBuffer.h"
#include "GraphicShader.h"

CMaterial::CMaterial()
	: CAsset(ASSET_TYPE::MATERIAL)
{
	//m_const.iArr[0] = 2;					// 분기처리로 제어하면 재질을 빨간색으로 만들어 피격 효과를 줄 수 있음
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
