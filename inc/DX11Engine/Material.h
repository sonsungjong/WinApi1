#pragma once
#include "Asset.h"

#include "assets.h"

class CMaterial
    : public CAsset
{
private:
    Ptr<CGraphicShader> m_shader;

    tMtrlConst m_const;
    Ptr<CTexture> m_arrTex[(UINT)TEX_PARAM::END];

public:
    CMaterial();
    virtual ~CMaterial();

    void setShader(Ptr<CGraphicShader> _shader) { m_shader = _shader; }
    Ptr<CGraphicShader> getShader() { return m_shader; }

    template<typename T>
    void setScalarParam(SCALAR_PARAM _paramType, const T& _value);

    void setTexParam(TEX_PARAM _param, Ptr<CTexture> _tex);

    void binding();

    virtual int load(const std::wstring& _strFilePath) { return S_OK; }
    virtual int save(const std::wstring& _strFilePath) { return S_OK; }
};

template<typename T>
inline void CMaterial::setScalarParam(SCALAR_PARAM _paramType, const T& _value)
{
    void* pValue = (void*)(&_value);             // 주소값을 받는다

    switch (_paramType)
    {
    case SCALAR_PARAM::INT_0:
    case SCALAR_PARAM::INT_1:
    case SCALAR_PARAM::INT_2:
    case SCALAR_PARAM::INT_3:
        m_const.iArr[(UINT)_paramType] = *((int*)pValue);                   // 주소를 형변환하여 역참조
        break;
    case SCALAR_PARAM::FLOAT_0:
    case SCALAR_PARAM::FLOAT_1:
    case SCALAR_PARAM::FLOAT_2:
    case SCALAR_PARAM::FLOAT_3:
        m_const.fArr[(UINT)_paramType - (UINT)SCALAR_PARAM::FLOAT_0] = *((float*)pValue);
        break;
    case SCALAR_PARAM::VEC2_0:
    case SCALAR_PARAM::VEC2_1:
    case SCALAR_PARAM::VEC2_2:
    case SCALAR_PARAM::VEC2_3:
        m_const.v2Arr[(UINT)_paramType - (UINT)SCALAR_PARAM::VEC2_0] = *((Vec2*)pValue);
        break;
    case SCALAR_PARAM::VEC4_0:
    case SCALAR_PARAM::VEC4_1:
    case SCALAR_PARAM::VEC4_2:
    case SCALAR_PARAM::VEC4_3:
        m_const.v4Arr[(UINT)_paramType - (UINT)SCALAR_PARAM::VEC4_0] = *((Vec4*)pValue);
        break;
    case SCALAR_PARAM::MAT_0:
    case SCALAR_PARAM::MAT_1:
    case SCALAR_PARAM::MAT_2:
    case SCALAR_PARAM::MAT_3:
        m_const.matArr[(UINT)_paramType - (UINT)SCALAR_PARAM::MAT_0] = *((Matrix*)pValue);
        break;
    default:
        break;
    }
}
