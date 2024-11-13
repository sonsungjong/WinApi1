#include "pch.h"
#include "framework.h"
#include "LoginWnd.h"

using namespace D2D1;
using namespace Microsoft::WRL;

HINSTANCE g_hInst;
HWND g_hWnd;

// D2D ��ü ���� ���丮
ComPtr<ID2D1Factory> gp_factory;

// D2D Ŭ���̾�Ʈ�� �׸��� �׸��� ��ü
ComPtr<ID2D1HwndRenderTarget> gp_render_target;

ComPtr<ID2D1Bitmap> gp_bitmap;

// �̹����� �ø� ����
D2D1_RECT_F g_img_rect;


void WndPAINT()
{
	::ValidateRect(g_hWnd, NULL);				// WM_PAINT�� �ٽ� �߻����� �ʰ� �Ѵ�
	gp_render_target->BeginDraw();

	// �̹����� ������ �̹����� ����� ����
	// �̹����� ������ Clear�Ѵ�
	if (gp_bitmap.Get() != NULL) {
		gp_render_target->Clear(ColorF(1.0f, 1.0f, 1.0f));						// �Ͼ������ �����
		gp_render_target->DrawBitmap(gp_bitmap.Get(), &g_img_rect);			// ����̹����� ���´�
	}
	else {
		gp_render_target->Clear(ColorF(1.0f, 1.0f, 1.0f));						// �Ͼ������ �����
	}

	gp_render_target->EndDraw();
}

void WndLBUTTONDOWN()
{

}

void WndDESTROY()
{
	// ComPtr ���� ���� ���� �ذ�
	gp_bitmap.Reset();
	gp_render_target.Reset();
	gp_factory.Reset();

	PostQuitMessage(0);			// WM_QUIT
}

void WndCREATE()
{
	::RECT rect;
	::GetClientRect(g_hWnd, &rect);			// ������ ��ǥ ��ü�� rect�� �ݿ��Ѵ�

	// rect�� ����Ÿ������ ���Ѵ�
	gp_factory->CreateHwndRenderTarget(RenderTargetProperties(), HwndRenderTargetProperties(g_hWnd, SizeU(rect.right, rect.bottom)), gp_render_target.GetAddressOf());

	// �̹������� ũ�⸦ ���Ѵ�
	g_img_rect.right = rect.right / 2;
	g_img_rect.bottom = rect.bottom / 2;

	BOOL load_result = FALSE;
	ComPtr<IWICImagingFactory> p_wic_factory;
	ComPtr<IWICBitmapFrameDecode> p_frame;
	ComPtr<IWICBitmapDecoder> p_decoder;
	ComPtr<IWICFormatConverter> p_converter;
	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(p_wic_factory.GetAddressOf()));

	if (S_OK == p_wic_factory->CreateDecoderFromFilename(L"res/google.png", NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, p_decoder.GetAddressOf()))
	{
		if (S_OK == p_decoder->GetFrame(0, p_frame.GetAddressOf()))
		{
			if (S_OK == p_wic_factory->CreateFormatConverter(p_converter.GetAddressOf()))
			{
				if (S_OK == p_converter->Initialize(p_frame.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom))
				{
					if (S_OK == gp_render_target->CreateBitmapFromWicBitmap(p_converter.Get(), NULL, gp_bitmap.GetAddressOf()))
					{
						load_result = TRUE;
					}
				}
			}
		}
	}
	
	// WIC ��ü ����
	p_converter.Reset();
	p_frame.Reset();
	p_decoder.Reset();
	p_wic_factory.Reset();

}


INT_PTR CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_PAINT:
		WndPAINT();
		break;
	case WM_LBUTTONDOWN:
		WndLBUTTONDOWN();
		break;
	case WM_DESTROY:
		WndDESTROY();
		break;
	case WM_CREATE:
		if (g_hWnd == NULL) {
			g_hWnd = hWnd;
		}
		WndCREATE();
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	TCHAR my_class_name[] = _T("main_wnd_class");
	g_hInst = hInstance;

	// D2D ���̺귯�� �ʱ�ȭ
	(void)CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	// D2D factory ����
	if (S_OK != D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, gp_factory.GetAddressOf())) {
		return 0;
	}

	WNDCLASS wc;

	wc.cbClsExtra = NULL;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = my_class_name;
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wc);

	g_hWnd = CreateWindow(my_class_name, _T("�α��� ȭ��"), WS_OVERLAPPEDWINDOW, 50, 50, 600, 600, NULL, NULL, hInstance, NULL);
	ShowWindow(g_hWnd, SW_SHOW);
	UpdateWindow(g_hWnd);

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// ���̺귯�� ����
	CoUninitialize();
	return (int)msg.wParam;
}