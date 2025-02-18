#pragma once
#include "Component.h"
class CCamera 
	: public CComponent
{
private:
	PROJ_TYPE m_ProjType;
	int m_nCamPriority;				// �����Ŵ����� ��ϵ� �� ī�޶��� �켱���� (0 : ����ī�޶�, 1~ : ����ī�޶�, -1 : ī�޶� �̻��)

	float m_FOV;								// field of view (�þ߰�)
	float m_far;								// �þ� �ִ�Ÿ�
	float m_width;							// �þ� ���α���
	float m_aspectRatio;					// ���μ��� ��Ⱦ ��

	float m_scale;							// �������� ����

	Matrix m_matView;				// �� ���
	Matrix m_matProj;					// ���� ���


public:
	CCamera();
	virtual ~CCamera();

	virtual void finaltick() override;
	void render();

	void setCameraPriority(int _priority);
	void setFat(float _far) { m_far = _far; }
	void setFOV(float _fov) { m_FOV = _fov; }
	void setWidth(float _width) { m_width = _width; }
	void setAspectRatio(float _ar) { m_aspectRatio = _ar; }
	void setProjType(PROJ_TYPE _type) { m_ProjType = _type; }				// ���� ���� : �Ÿ��� ���� ũ�Ⱑ �޶���, ���� ���� : �Ÿ��� ������� ������ ũ�� (2D ����)
};

