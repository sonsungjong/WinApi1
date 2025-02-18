#ifndef DEVICE_H_
#define DEVICE_H_

// 전방선언
class CConstBuffer;

class CDevice
	: public CSingleton<CDevice>
{
	SINGLE(CDevice)
private:
	HWND m_hWnd;			// 출력 대상 윈도우
	Vec2 m_render_resolution;			// 렌더링 해상도

	//ComPtr<ID3D11Device> 로 사용하면 초기화 및 Release를 안해줘도 됨
	ID3D11Device* m_device;							// GPU 메모리 할당, DX11 관련 객체 생성
	ID3D11DeviceContext* m_context;				// GPU 렌더링 관련 명령

	IDXGISwapChain* m_swapChain;			// 렌더타겟 버퍼 소유, 화면에 최종 장면을 게시(Present)
	
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
	int createConstBuffer();				// 상수버퍼 초기화
	int createSamplerState();
	int createRasterizerState();
	int createDepthStencilState();

public:
	int init(HWND _hWnd, Vec2 _resolution);
	void clearTarget(float(&_ArrColor)[4]);				// 배열 4개 짜리를 받는다 (해당 색상으로 지움)
	void present();

	Vec2 getRenderResolution() { return m_render_resolution; }

	ID3D11Device* GetDivice() { return m_device; }
	ID3D11DeviceContext* GetContext() { return m_context; }

	CConstBuffer* getConstBuffer(CB_TYPE _type) { return m_CB[(UINT)_type]; }
	ComPtr<ID3D11RasterizerState> getRasterizerState(RS_TYPE _type) { return m_arrRasterizerState[(UINT)_type]; }

//public:
	// 간략화버전 싱글턴패턴
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