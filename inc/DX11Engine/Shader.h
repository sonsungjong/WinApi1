#pragma once

#include "Asset.h"

// �� ���̴��鿡 ����� �߻�Ŭ����
class CShader
	: public CAsset
{
protected:
	ComPtr<ID3DBlob> m_ErrBlob;				// ���� �θ��� ���

public:
	virtual void binding() = 0;				// ���� ���� �Լ� (���� ����� ��ӹ��� �ڽ� Ŭ�������� �� ����������ϴ� �Լ� ���)

public:
	CShader(ASSET_TYPE _type);
	~CShader();
};

