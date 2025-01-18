#pragma once
#include "Component.h"

#include "assets.h"

class CRenderComponent :
    public CComponent
{
private:
    Ptr<CMesh> m_mesh;
    Ptr<CGraphicShader> m_shader;
    Ptr<CTexture> m_tex;

public:
    virtual void render() = 0;

    void setMesh(Ptr<CMesh> _mesh) { m_mesh = _mesh; }
    void setShader(Ptr<CGraphicShader> _shader) { m_shader = _shader; }
    void setTexture(Ptr<CTexture> _tex) { m_tex = _tex; }

    // 자식에서 접근할 수 있게 get함수 만든다
    Ptr<CMesh> getMesh() { return m_mesh; }
    Ptr<CGraphicShader> getShader() { return m_shader; }
    Ptr<CTexture> getTexture() { return m_tex; }

    CRenderComponent(COMPONENT_TYPE _type);
    virtual ~CRenderComponent();

};

