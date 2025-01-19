#pragma once
#include "Shader.h"

// 그래픽 쉐이더 : 렌더링을 위한 쉐이더 (버텍스 쉐이더 + 픽셀 쉐이더)

class CGraphicShader :
    public CShader
{
private:
    // 쉐이더를 컴파일 했을 때 컴파일된 코드를 저장하는 블롭
    ComPtr<ID3DBlob> m_pVertexShaderBlob;
    // 픽쉘 세이더를 컴파일해서 생성된 코드를 저장하고 있는 픽셀 쉐이더 블롭
    ComPtr<ID3DBlob> m_pPixelShaderBlob;

    // 버텍스 쉐이더 블롭으로 컴파일해서 생성시킨 버텍스 쉐이더 객체
    ComPtr<ID3D11VertexShader> m_pVertexShader;
    // 픽셀 쉐이더 블롭으로 만들어진 픽셀 쉐이더 객체
    ComPtr<ID3D11PixelShader> m_pPixelShader;

    // 정점의 레이아웃 정보 (시멘틱의 정점 구조 정보)
    ComPtr<ID3D11InputLayout> m_pLayout;

    // 정점 몇개가 면인지 식별하는 위상구조
    D3D11_PRIMITIVE_TOPOLOGY m_Topology;

    RS_TYPE m_enumRasterizerType;

public:
    int createVertexShader(const std::wstring& _wstrFilePath, const std::string& _strVSFuncName);
    int createPixelShader(const std::wstring& _wstrFilePath, const std::string& _strPSFuncName);
    void setTopology(D3D11_PRIMITIVE_TOPOLOGY _Topology) { m_Topology = _Topology; }
    void setRasterizerStateType(RS_TYPE _type) { m_enumRasterizerType = _type; }

    virtual void binding() override;

public:
    CGraphicShader();
    ~CGraphicShader();
};

