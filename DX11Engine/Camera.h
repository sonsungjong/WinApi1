#pragma once
#include "Component.h"
class CCamera 
	: public CComponent
{
private:
	int m_nCamPriority;				// �����Ŵ����� ��ϵ� �� ī�޶��� �켱���� (0 : ����ī�޶�, 1~ : ����ī�޶�, -1 : ī�޶� �̻��)

	Matrix m_matView;				// �� ���
	Matrix m_matProj;					// ���� ���

	float m_far;

public:
	CCamera();
	virtual ~CCamera();

	virtual void finaltick() override;
	void render();

	void setCameraPriority(int _priority);
};

