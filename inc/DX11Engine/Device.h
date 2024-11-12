#ifndef DEVICE_H_
#define DEVICE_H_
class CDevice
{
private:
	HWND m_hWnd;			// ��� ��� ������
	POINT m_render_resolution;			// ������ �ػ�

	ID3D11Device* m_device;							// GPU �޸� �Ҵ�, DX11 ���� ��ü ����
	ID3D11DeviceContext* m_context;				// GPU ������ ���� ���

	IDXGISwapChain* m_swapChain;			// ����Ÿ�� ���� ����, ȭ�鿡 ���� ����� �Խ�(Present)
	
	ID3D11Texture2D* m_renderTargetTex;
	ID3D11RenderTargetView* m_RTV;

	ID3D11Texture2D* m_depthStencilTex;
	ID3D11DepthStencilView* m_DSV;

public:
	int init(HWND _hWnd, POINT _resolution);

private:
	int createSwapChain();
	int createView();

public:
	// ����ȭ���� �̱�������
	static CDevice* getInstance()
	{
		static CDevice mgr;
		return &mgr;
	}

private:
	CDevice();
	CDevice(const CDevice& _other) = delete;
	CDevice operator=(const CDevice& _other) = delete;

public:
	virtual ~CDevice();

};

#endif		DEVICE_H_