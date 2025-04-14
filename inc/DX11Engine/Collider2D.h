#pragma once
#include "Component.h"
class CCollider2D
    : public CComponent
{
private:
    // 2D�浹ü�� ������ ����� �������ϳ�?
    Vec3 m_offset;                      // ������ ������
    Vec3 m_finalPos;                    // �浹ü�� ���� ��ġ (��ü��ġ + ������)
    
    Vec3 m_scale;                     // ������Ʈ ũ�⿡ ���� ������� ����
    Vec3 m_rotation;                // ������Ʈ�κ��� ������� ȸ����(����)
    bool m_bAbsolute;               // [true] ������Ʈ ũ�⿡ ������ �����ʴ� ����ũ��� ���

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

