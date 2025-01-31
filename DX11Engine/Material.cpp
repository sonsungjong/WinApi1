#include "pch.h"
#include "Material.h"
#include "Device.h"
#include "ConstBuffer.h"
#include "GraphicShader.h"

CMaterial::CMaterial()
	: CAsset(ASSET_TYPE::MATERIAL)
{
	
}

CMaterial::~CMaterial()
{

}

void CMaterial::setTexParam(TEX_PARAM _param, Ptr<CTexture> _tex)
{
	m_arrTex[(UINT)_param] = _tex;
}

void CMaterial::binding()
{
	// 텍스처 바인딩
	for (UINT i = 0U; i < static_cast<UINT>(TEX_PARAM::END); ++i) {
		if (nullptr == m_arrTex[i]) 
		{
			// 이전에 셋팅한게 있으면 클리어
			CTexture::clear(i);
			continue;
		}
		
		m_arrTex[i]->binding(i);
		
	}

	// 상수 데이터 바인딩
	if (m_shader.Get() != nullptr)
	{
		m_shader->binding();
	}

	CConstBuffer* pMtrlCB = CDevice::getInstance()->getConstBuffer(CB_TYPE::MATERIAL);
	pMtrlCB->setData(&m_const);
	pMtrlCB->binding();
}
