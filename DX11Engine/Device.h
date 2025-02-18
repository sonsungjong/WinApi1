#ifndef DEVICE_H_
#define DEVICE_H_

// ���漱��
class CConstBuffer;

class CDevice
	: public CSingleton<CDevice>
{
	SINGLE(CDevice)
private:
	HWND m_hWnd;			// ��� ��� ������
	Vec2 m_render_resolution;			// ������ �ػ�

	//ComPtr<ID3D11Device> �� ����ϸ� �ʱ�ȭ �� Release�� �����൵ ��
	ID3D11Device* m_device;							// GPU �޸� �Ҵ�, DX11 ���� ��ü ����
	ID3D11DeviceContext* m_context;				// GPU ������ ���� ���

	IDXGISwapChain* m_swapChain;			// ����Ÿ�� ���� ����, ȭ�鿡 ���� ����� �Խ�(Present)
	
	ID3D11Texture2D* m_renderTargetTex;
	ID3D11RenderTargetView* m_RTV;

	ID3D11Texture2D* m_depthStencilTex;
	ID3D11DepthStencilView* m_DSV;

	ComPtr<ID3D11SamplerState> m_arrSamplerState[2];
	ComPtr<ID3D11RasterizerState> m_arrRasterizerState[(UINT)RS_TYPE::END];
	//ComPtr<ID3D11BlendState> m_BS[];
	ComPtr<ID3D11DepthStencilState> m_DS[(UINT)DS_TYPE::END];
	CConstBuffer* m_CB[(UINT)CB_TYPE::END];

private:
	int createSwapChain();
	int createView();
	int createConstBuffer();				// ������� �ʱ�ȭ
	int createSamplerState();
	int createRasterizerState();
	int createDepthStencilState();

public:
	int init(HWND _hWnd, Vec2 _resolution);
	void clearTarget(float(&_ArrColor)[4]);				// �迭 4�� ¥���� �޴´� (�ش� �������� ����)
	void present();

	Vec2 getRenderResolution() { return m_render_resolution; }

	ID3D11Device* GetDivice() { return m_device; }
	ID3D11DeviceContext* GetContext() { return m_context; }

	CConstBuffer* getConstBuffer(CB_TYPE _type) { return m_CB[(UINT)_type]; }
	ComPtr<ID3D11RasterizerState> getRasterizerState(RS_TYPE _type) { return m_arrRasterizerState[(UINT)_type]; }

//public:
	// ����ȭ���� �̱�������
	//static CDevice* getInstance()
	//{
	//	static CDevice mgr;
	//	return &mgr;
	//}
//private:
//	CDevice();
//	CDevice(const CDevice& _other) = delete;
//	CDevice operator=(const CDevice& _other) = delete;
//
//public:
//	virtual ~CDevice();

};

#endif		DEVICE_H_