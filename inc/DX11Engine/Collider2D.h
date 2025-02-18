#pragma once
#include "Component.h"
class CCollider2D
    : public CComponent
{
private:
    // 2D충돌체는 무엇을 멤버로 가져야하나?
    Vec3 m_offset;                      // 오프셋 포지션
    Vec3 m_finalPos;                    // 충돌체의 최종 위치 (물체위치 + 오프셋)
    
    Vec3 m_scale;                     // 오브젝트 크기에 따른 상대적인 배율
    Vec3 m_rotation;                // 오브젝트로부터 상대적인 회전량(자전)
    bool m_bAbsolute;               // [true] 오브젝트 크기에 영향을 받지않는 고정크기로 계산

    Matrix m_matColWorld;

public:
    CCollider2D();
    virtual ~CCollider2D();

    virtual void finaltick() override;

    void setOffset(Vec3 _vOffset) { m_offset = _vOffset; }
    void setScale(Vec3 _vScale) { m_scale = _vScale; }
    void setRotationZ(float _angle) { m_rotation.z = _angle; }
    
    Vec3 getOffset() { return m_offset; }
    Vec3 getScale() { return m_scale; }
    Vec3 getFinalPos() { return m_finalPos; }
    float getRotationZ() { return m_rotation.z; }

    void setAbsolute(bool _abs) { m_bAbsolute = _abs; }
    bool isAbsolute() { return m_bAbsolute; }
};

