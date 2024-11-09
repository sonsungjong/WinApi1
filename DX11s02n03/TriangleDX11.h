#ifndef TRIANGLE_DX11_H_
#define TRIANGLE_DX11_H_

#include "Dx11Base.h"

class CTriangleDX11 : public CDx11Base
{
// Constructors
public:
	CTriangleDX11();
	virtual ~CTriangleDX11();

// Overrides
public:
	virtual bool LoadContent();
	virtual void UnloadContent();

	virtual void Update();
	virtual void Render();

// members
protected:
	ID3D11VertexShader* m_pVSGreenColor;
	ID3D11PixelShader* m_pPSGreenColor;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11Buffer* m_pVertexBuffer;

};

#endif		TRIANGLE_DX11_H_