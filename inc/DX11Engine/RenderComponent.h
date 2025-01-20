#pragma once
#include "Component.h"

#include "assets.h"

class CRenderComponent :
    public CComponent
{
private:
    Ptr<CMesh> m_mesh;
    //Ptr<CGraphicShader> m_shader;
    //Ptr<CTexture> m_tex;
    Ptr<CMaterial> m_material;

public:
    virtual void render() = 0;

    void setMesh(Ptr<CMesh> _mesh) { m_mesh = _mesh; }
    void setMaterial(Ptr<CMaterial> _Mtrl) { m_material = _Mtrl; }

    // �ڽĿ��� ������ �� �ְ� get�Լ� �����
    Ptr<CMesh> getMesh() { return m_mesh; }
    Ptr<CMaterial> getMaterial() { return m_material; }

    CRenderComponent(COMPONENT_TYPE _type);
    virtual ~CRenderComponent();

};

