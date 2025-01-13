#pragma once

#include "Asset.h"

// 각 쉐이더들에 상속할 추상클래스
class CShader
	: public CAsset
{
protected:
	ComPtr<ID3DBlob> m_ErrBlob;				// 공통 부모의 멤버

public:
	virtual void binding() = 0;				// 순수 가상 함수 (실제 사용할 상속받은 자식 클래스에서 꼭 구현해줘야하는 함수 명시)

public:
	CShader(ASSET_TYPE _type);
	~CShader();
};

