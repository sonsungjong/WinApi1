#ifndef DX11Base_H_
#define DX11Base_H_

#include <d3d11.h>
#include <dxgi.h>

class CDx11Base
{
// Constructors
public:
	CDx11Base();
	virtual ~CDx11Base();

// methods
public:
	bool Initialize(HWND hWnd, HINSTANCE hInst);
	void Terminate();

// Overrides
public:
	virtual bool LoadContent() = 0;
	virtual void UnloadContent() = 0;

	virtual void Update() = 0;
	virtual void Render() = 0;

protected:
	HWND m_hWnd;
	HINSTANCE m_hInst;
	ID3D11Device* m_pD3DDevice;
	ID3D11DeviceContext* m_pD3DContext;
	ID3D11RenderTargetView* m_pD3DRenderTargetView;
	IDXGISwapChain* m_pSwapChain;
};

#endif		DX11Base_H_