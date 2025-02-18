#pragma once
#include "Component.h"
class CCamera 
	: public CComponent
{
private:
	PROJ_TYPE m_ProjType;
	int m_nCamPriority;				// 렌더매니저에 등록될 때 카메라의 우선순위 (0 : 메인카메라, 1~ : 서브카메라, -1 : 카메라 미사용)

	float m_FOV;								// field of view (시야각)
	float m_far;								// 시야 최대거리
	float m_width;							// 시야 가로길이
	float m_aspectRatio;					// 가로세로 종횡 비

	float m_scale;							// 직교투영 배율

	Matrix m_matView;				// 뷰 행렬
	Matrix m_matProj;					// 투영 행렬


public:
	CCamera();
	virtual ~CCamera();

	virtual void finaltick() override;
	void render();

	void setCameraPriority(int _priority);
	void setProjType(PROJ_TYPE _type) { m_ProjType = _type; }				// 원근 투영 : 거리에 따라 크기가 달라짐, 직교 투영 : 거리에 상관없이 동일한 크기 (2D 개발)
	void setFat(float _far) { m_far = _far; }
	void setFOV(float _fov) { m_FOV = _fov; }
	void setWidth(float _width) { m_width = _width; }
	void setAspectRatio(float _ar) { m_aspectRatio = _ar; }
	void setScale(float _scale) { m_scale = _scale; }

	int getCameraPriority() { return m_nCamPriority; }
	PROJ_TYPE getProjType() { return m_ProjType; }
	float getFat() { return m_far; }
	float getFOV() { return m_FOV; }
	float getWidth() { return m_width; }
	float getAspectRatio() { return m_aspectRatio; }
	float getScale() { return m_scale; }
};

