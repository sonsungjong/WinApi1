#include "pch.h"
#include "Shader.h"

// 순수 가상 함수로 구성된 추상클래스 : 자식에게 꼭 전달해야하는 것만 가상함수로 구현

CShader::CShader(ASSET_TYPE _type)
	: CAsset(_type)					// 부모생성자에 올려준다
{

}

CShader::~CShader()
{
}
