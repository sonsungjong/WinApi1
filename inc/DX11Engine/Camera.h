#pragma once
#include "Component.h"
class CCamera 
	: public CComponent
{
private:
	int m_nCamPriority;				// 렌더매니저에 등록될 때 카메라의 우선순위 (0 : 메인카메라, 1~ : 서브카메라, -1 : 카메라 미사용)

	Matrix m_matView;				// 뷰 행렬
	Matrix m_matProj;					// 투영 행렬

	float m_far;

public:
	CCamera();
	virtual ~CCamera();

	virtual void finaltick() override;
	void render();

	void setCameraPriority(int _priority);
};

