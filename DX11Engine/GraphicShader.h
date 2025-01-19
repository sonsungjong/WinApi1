#pragma once
#include "Shader.h"

// �׷��� ���̴� : �������� ���� ���̴� (���ؽ� ���̴� + �ȼ� ���̴�)

class CGraphicShader :
    public CShader
{
private:
    // ���̴��� ������ ���� �� �����ϵ� �ڵ带 �����ϴ� ���
    ComPtr<ID3DBlob> m_pVertexShaderBlob;
    // �Ƚ� ���̴��� �������ؼ� ������ �ڵ带 �����ϰ� �ִ� �ȼ� ���̴� ���
    ComPtr<ID3DBlob> m_pPixelShaderBlob;

    // ���ؽ� ���̴� ������� �������ؼ� ������Ų ���ؽ� ���̴� ��ü
    ComPtr<ID3D11VertexShader> m_pVertexShader;
    // �ȼ� ���̴� ������� ������� �ȼ� ���̴� ��ü
    ComPtr<ID3D11PixelShader> m_pPixelShader;

    // ������ ���̾ƿ� ���� (�ø�ƽ�� ���� ���� ����)
    ComPtr<ID3D11InputLayout> m_pLayout;

    // ���� ��� ������ �ĺ��ϴ� ������
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

