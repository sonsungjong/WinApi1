#pragma once
#include "Shader.h"

// ��ǻƮ ���̴� : GPGPU �뷮 ����ó�� ����� ���̴� (General Purpose GPU, ���� ������ ����� GPU, ��Ʈ���� ��)

class CComputeShader :
    public CShader
{
public:
    CComputeShader();
    ~CComputeShader();
};

