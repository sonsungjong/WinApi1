#include "pch.h"
#include "CollisionMgr.h"

CCollisionMgr::CCollisionMgr()
{

}

CCollisionMgr::~CCollisionMgr()
{

}

void CCollisionMgr::tick()
{
	for (UINT row = 0; row < MAX_LAYER; ++row)
	{
		for (UINT col = row; col < MAX_LAYER; ++col) 
		{
			if (m_matrix[row] & (1 << col))
			{
				int a = 0;
			}
		}
	}
}

void CCollisionMgr::LayerCheck(UINT _LayerLeftIdx, UINT _LayerRightIdx)
{
	// �ε����� �� �Ǵ� ���� �ȴ�			// �� ���� ���� Row�� ����
	UINT row = _LayerLeftIdx;
	UINT col = _LayerRightIdx;

	if (col < row) 
	{
		row = _LayerRightIdx;
		col = _LayerLeftIdx;
	}

	UINT checkBit = (1 << col);

	if (m_matrix[row] & checkBit)
	{
		m_matrix[row] &= ~checkBit;
	}
	else
	{
		m_matrix[row] |= checkBit;
	}
}
