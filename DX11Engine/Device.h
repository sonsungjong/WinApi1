#ifndef DEVICE_H_
#define DEVICE_H_
class CDevice
{
private:
	HWND m_hWnd;			// 출력 대상 윈도우
	POINT m_render_resolution;			// 렌더링 해상도

	ID3D11Device* m_device;							// GPU 메모리 할당, DX11 관련 객체 생성
	ID3D11DeviceContext* m_context;				// GPU 렌더링 관련 명령

	IDXGISwapChain* m_swapChain;			// 렌더타겟 버퍼 소유, 화면에 최종 장면을 게시(Present)
	
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
	// 간략화버전 싱글턴패턴
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