#pragma once
#include "Shader.h"

// 컴퓨트 쉐이더 : GPGPU 대량 병렬처리 연산용 쉐이더 (General Purpose GPU, 범용 목적의 연산용 GPU, 비트코인 등)

class CComputeShader :
    public CShader
{
public:
    CComputeShader();
    ~CComputeShader();
};

