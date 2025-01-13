#include "pch.h"
#include "ComputeShader.h"

CComputeShader::CComputeShader()
	: CShader(ASSET_TYPE::COMPUTE_SHADER)				// 해당 클래스가 컴퓨트 쉐이더 임을 부모생성자에게 알려줌
{
}

CComputeShader::~CComputeShader()
{
}
