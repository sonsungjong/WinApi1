#pragma once
#include "Component.h"
class CCamera 
	: public CComponent
{
private:
	Matrix m_matView;

public:
	CCamera();
	virtual ~CCamera();
};

