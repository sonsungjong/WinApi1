#pragma once
#include "Component.h"
class CTransform :
    public CComponent
{
private:
    Vec3 m_RelativePos;                     // 위치
    Vec3 m_RelativeScale;                   // 크기 배율
    Vec3 m_RelativeRotation;                // 회전
    Vec3 m_RelativeDirection[(UINT)DIR_TYPE::END];                // 방향 벡터 3개



    Matrix m_matWorld;                  // 월드 행렬 (4x4)

public:
    CTransform();
    virtual ~CTransform();

    virtual void finaltick() override;
    void binding();

    Vec3 getRelativePos() { return m_RelativePos; }
    Vec3 getRelativeScale() { return m_RelativeScale; }
    Vec3 getRelativeRotation() { return m_RelativeRotation; }

    void setRelativePos(const Vec3& _vPos) { m_RelativePos = _vPos; }
    void setRelativeScale(const Vec3& _vScale) { m_RelativeScale = _vScale; }
    void setRelativeRotation(const Vec3& _vRotation) { m_RelativeRotation = _vRotation; }

    void setRelativePos(float _x, float _y, float _z) { m_RelativePos = Vec3(_x, _y, _z); }
    void setRelativeScale(float _x, float _y, float _z) { m_RelativeScale = Vec3(_x, _y, _z); }
    void setRelativeRotation(float _x, float _y, float _z) { m_RelativeRotation = Vec3(_x, _y, _z); }

    void setWorldMatrix(const Matrix& _mat) { m_matWorld = _mat; }
    const Matrix& getWorldMatrix() { return m_matWorld; }

    Vec3 getRelativeDirection(DIR_TYPE _type) { return m_RelativeDirection[(UINT)_type]; }
};

