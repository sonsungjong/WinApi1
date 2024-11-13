#include "pch.h"
#include "framework.h"
#include "LoginWnd.h"

using namespace D2D1;
using namespace Microsoft::WRL;

HINSTANCE g_hInst;
HWND g_hWnd;

ComPtr<ID2D1Factory> gp_factory;						// D2D 객체 생성 팩토리 (프로세스 하나)
ComPtr<IDWriteFactory> gp_writeFactory;				// DWrite 객체 (프로세스 하나)


// 각 wnd마다 생성되야하는 객체
// D2D 클라이언트에 그림을 그리는 객체
ComPtr<ID2D1HwndRenderTarget> gp_render_target;

// 이미지를 올릴 변수
ComPtr<ID2D1Bitmap> gp_bitmap;
D2D1_RECT_F g_img_rect;


// 버튼
ComPtr<ID2D1SolidColorBrush> gp_buttonNormalBrush;
ComPtr<ID2D1SolidColorBrush> gp_buttonPushedBrush;
ComPtr<ID2D1SolidColorBrush> gp_buttonTextNormalBrush;
ComPtr<ID2D1SolidColorBrush> gp_buttonTextPushedBrush;
ComPtr<IDWriteTextFormat> gp_buttonTextFormat;					// 버튼

D2D1_RECT_F g_buttonNormalRect = {  };
D2D1_RECT_F g_buttonPushedRect = {  };
D2D1_ROUNDED_RECT g_buttonNormalRoundRect = {  };			// 보통상태
D2D1_ROUNDED_RECT g_buttonPushedRoundRect = {  };			// 눌렸을때 살짝 우측하단으로 이동해서 눌린 느낌을 준다
bool g_isButtonPushed = false;

// 에디트 컨트롤 묶음
bool g_isKoreanMode = false;

// 에디트 컨트롤1
ComPtr<IDWriteTextFormat> gp_edit1TextFormat;					// 에디트 컨트롤
bool g_isFocusedEdit1 = false;
D2D1_RECT_F g_edit1Rect = {  };
D2D1_ROUNDED_RECT g_edit1RoundRect = {  };
ComPtr<ID2D1SolidColorBrush> gp_edit1BorderBrush;
ComPtr<ID2D1SolidColorBrush> gp_edit1BorderFocusedBrush;
ComPtr<ID2D1SolidColorBrush> gp_edit1BackBrush;
ComPtr<ID2D1SolidColorBrush> gp_edit1TextBrush;
ComPtr<ID2D1SolidColorBrush> gp_edit1CursorBrush;
wchar_t wstr_edit1[255]{ 0 };
wchar_t wstr_edit1_view[256]{ 0 };
wchar_t edit1_dump;

// 에디트 컨트롤2
ComPtr<IDWriteTextFormat> gp_edit2TextFormat;					// 에디트 컨트롤
bool g_isFocusedEdit2 = false;
D2D1_RECT_F g_edit2Rect = {  };
D2D1_ROUNDED_RECT g_edit2RoundRect = {  };
ComPtr<ID2D1SolidColorBrush> gp_edit2BorderBrush;
ComPtr<ID2D1SolidColorBrush> gp_edit2BorderFocusedBrush;
ComPtr<ID2D1SolidColorBrush> gp_edit2BackBrush;
ComPtr<ID2D1SolidColorBrush> gp_edit2TextBrush;
ComPtr<ID2D1SolidColorBrush> gp_edit2CursorBrush;
wchar_t wstr_edit2[255]{ 0 };
wchar_t wstr_edit2_view[256]{ 0 };
wchar_t edit2_dump;

void DrawButton()
{
	const wchar_t* buttonText = L"로그인";

	if (g_isButtonPushed == false) {
		gp_render_target->FillRoundedRectangle(g_buttonNormalRoundRect, gp_buttonNormalBrush.Get());
		gp_render_target->DrawText(buttonText, wcslen(buttonText), gp_buttonTextFormat.Get(), g_buttonNormalRect, gp_buttonTextNormalBrush.Get());
	}
	else {
		gp_render_target->FillRoundedRectangle(g_buttonPushedRoundRect, gp_buttonPushedBrush.Get());
		gp_render_target->DrawText(buttonText, wcslen(buttonText), gp_buttonTextFormat.Get(), g_buttonPushedRect, gp_buttonTextPushedBrush.Get());
	}
}

void DrawEditControl()
{
	// 배경과 테두리
	//gp_render_target->FillRectangle(g_cursorPos, gp_editCursorBrush.Get());				// 커서

	if (!g_isFocusedEdit1) {
		gp_render_target->FillRoundedRectangle(g_edit1RoundRect, gp_edit1BackBrush.Get());
		gp_render_target->DrawRoundedRectangle(g_edit1RoundRect, gp_edit1BorderBrush.Get());
		gp_render_target->DrawText(wstr_edit1_view, wcslen(wstr_edit1_view), gp_edit1TextFormat.Get(), g_edit1Rect, gp_edit1TextBrush.Get());
	}
	else {
		gp_render_target->FillRoundedRectangle(g_edit1RoundRect, gp_edit1BackBrush.Get());
		gp_render_target->DrawRoundedRectangle(g_edit1RoundRect, gp_edit1BorderFocusedBrush.Get());
		//gp_render_target->FillRectangle(g_editCursorRect, gp_editCursorBrush.Get());
		gp_render_target->DrawText(wstr_edit1_view, wcslen(wstr_edit1_view), gp_edit1TextFormat.Get(), g_edit1Rect, gp_edit1TextBrush.Get());
	}

	if (!g_isFocusedEdit2) {
		gp_render_target->FillRoundedRectangle(g_edit2RoundRect, gp_edit2BackBrush.Get());
		gp_render_target->DrawRoundedRectangle(g_edit2RoundRect, gp_edit2BorderBrush.Get());
		gp_render_target->DrawText(wstr_edit2_view, wcslen(wstr_edit2_view), gp_edit2TextFormat.Get(), g_edit2Rect, gp_edit2TextBrush.Get());
	}
	else {
		gp_render_target->FillRoundedRectangle(g_edit2RoundRect, gp_edit2BackBrush.Get());
		gp_render_target->DrawRoundedRectangle(g_edit2RoundRect, gp_edit2BorderFocusedBrush.Get());
		//gp_render_target->FillRectangle(g_edit2CursorRect, gp_edit2CursorBrush.Get());
		gp_render_target->DrawText(wstr_edit2_view, wcslen(wstr_edit2_view), gp_edit2TextFormat.Get(), g_edit2Rect, gp_edit2TextBrush.Get());
	}
}



void WndCHAR(wchar_t ch)
{
	if (g_isFocusedEdit1)
	{
		int nLen = lstrlen(wstr_edit1);
		switch (ch) {
		case 8:
			if (nLen > 0) {
				wstr_edit1[nLen - 1] = 0;
				wcscpy(wstr_edit1_view, wstr_edit1);
				gp_render_target->DrawText(wstr_edit1_view, wcslen(wstr_edit1_view), gp_edit1TextFormat.Get(), g_edit1Rect, gp_edit1TextBrush.Get());
				WndPAINT();				// 텍스트를 화면에 반영
			}
				break;
		default:
			if (nLen < 255) {
				wstr_edit1[nLen] = ch;
				wcscpy(wstr_edit1_view, wstr_edit1);
				gp_render_target->DrawText(wstr_edit1_view, wcslen(wstr_edit1_view), gp_edit1TextFormat.Get(), g_edit1Rect, gp_edit1TextBrush.Get());
				WndPAINT();				// 텍스트를 화면에 반영
			}
			break;
		}
		edit1_dump = L'\0';
	}
	else if (g_isFocusedEdit2) {
		int nLen = lstrlen(wstr_edit2);
		switch (ch) {
		case 8:
			if (nLen > 0) {
				wstr_edit2[nLen - 1] = 0;
				wcscpy(wstr_edit2_view, wstr_edit2);
				gp_render_target->DrawText(wstr_edit2_view, wcslen(wstr_edit2_view), gp_edit2TextFormat.Get(), g_edit2Rect, gp_edit2TextBrush.Get());
				WndPAINT();				// 텍스트를 화면에 반영
			}
			break;
		default:
			if (nLen < 255) {
				wstr_edit2[nLen] = ch;
				wcscpy(wstr_edit2_view, wstr_edit2);
				gp_render_target->DrawText(wstr_edit2_view, wcslen(wstr_edit2_view), gp_edit2TextFormat.Get(), g_edit2Rect, gp_edit2TextBrush.Get());
				WndPAINT();				// 텍스트를 화면에 반영
			}
			break;
		}
		edit1_dump = L'\0';
	}
}

// 한글백업UI 미전시
void disableIMMContext() {
	HIMC hIMC = ImmGetContext(g_hWnd);
	if (hIMC) {
		COMPOSITIONFORM compForm;
		compForm.dwStyle = CFS_EXCLUDE;
		compForm.ptCurrentPos.x = 0;
		compForm.ptCurrentPos.y = 0;

		::RECT rc;
		GetClientRect(g_hWnd, &rc);
		compForm.rcArea = rc;
		ImmSetCompositionWindow(hIMC, &compForm);
		ImmReleaseContext(g_hWnd, hIMC);

	}
}



void WndPAINT()
{
	gp_render_target->BeginDraw();

	// 이미지가 있으면 이미지로 배경을 덮고
	// 이미지가 없으면 Clear한다
	if (gp_bitmap.Get() != NULL) {
		gp_render_target->Clear(ColorF(1.0f, 1.0f, 1.0f));						// 하얀색으로 지운다
		gp_render_target->DrawBitmap(gp_bitmap.Get(), &g_img_rect);			// 비트맵에 이미지상자를 그린다
	}
	else {
		gp_render_target->Clear(ColorF(1.0f, 1.0f, 1.0f));						// 하얀색으로 지운다
	}

	// 등록된 모든 버튼 그리기
	DrawButton();

	// 등록된 모든 에디트 컨트롤 그리기
	DrawEditControl();

	gp_render_target->EndDraw();
}

void WndLBUTTONDOWN(int x, int y)
{
	// 선택한 버튼의 인덱스를 선택여부 id에 백업하고
	// 선택한 버튼의 플래그를 바꾸고
	// 해당 영역만 다시 그려지도록 갱신시키고
	g_isFocusedEdit1 = false;
	g_isFocusedEdit2 = false;
	if (x >= g_buttonNormalRect.left && x <= g_buttonNormalRect.right && y >= g_buttonNormalRect.top && y <= g_buttonNormalRect.bottom) {
		g_isButtonPushed = true;
	}
	else if (x >= g_edit1Rect.left && x <= g_edit1Rect.right && y >= g_edit1Rect.top && y <= g_edit1Rect.bottom) {
		g_isFocusedEdit1 = true;
	}
	else if (x >= g_edit2Rect.left && x <= g_edit2Rect.right && y >= g_edit2Rect.top && y <= g_edit2Rect.bottom) {
		g_isFocusedEdit2 = true;
	}
}

void WndMOUSEMOVE(int x, int y) {
	
}

// 선택여부 인덱스에 해당하는 버튼이 있는지 체크하고
// 해당 버튼의 플래그가 true이고 버튼의 영역안에 있으면
// 등록한 콜백함수를 동작시킨다 (콜백으로할지, PostMessage로 할지 미정)
void WndLBUTTONUP(int x, int y) {
	if (g_isButtonPushed == true && x >= g_buttonNormalRect.left && x <= g_buttonNormalRect.right && y >= g_buttonNormalRect.top && y <= g_buttonNormalRect.bottom) {
		MessageBox(g_hWnd, L"버튼 눌림", L"버튼", MB_OK);		// 선택id 버튼에 대한 함수발동
	}
	g_isButtonPushed = false;			// 선택 버튼에 대한 클릭플래그 false 처리
}

void WndDESTROY()
{
	// ComPtr 해제 순서 문제 해결
	gp_bitmap.Reset();
	gp_render_target.Reset();
	gp_factory.Reset();

	PostQuitMessage(0);			// WM_QUIT
}

void WndCREATE()
{
	::RECT wnd_rect;
	::GetClientRect(g_hWnd, &wnd_rect);			// 윈도우 좌표 전체를 rect에 반영한

	// wnd_rect를 렌더타겟의 영역으로 정한다
	gp_factory->CreateHwndRenderTarget(RenderTargetProperties(), HwndRenderTargetProperties(g_hWnd, SizeU(wnd_rect.right, wnd_rect.bottom)), gp_render_target.GetAddressOf());
	D2D1_RECT_F excludeRect = D2D1::RectF(100, 100, 300, 180); // Edit 컨트롤 영역

	gp_render_target->BeginDraw();
	gp_render_target->PushAxisAlignedClip(excludeRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	gp_render_target->Clear(ColorF(1.0f, 1.0f, 1.0f)); // 흰색으로 Clear
	gp_render_target->PopAxisAlignedClip();
	gp_render_target->EndDraw();

	// 이미지상자 크기를 정한다
	g_img_rect.left = wnd_rect.right / 3.f;
	g_img_rect.top = wnd_rect.bottom * 25.f/100.f;
	g_img_rect.right = wnd_rect.right *2.f/3.f;
	g_img_rect.bottom = wnd_rect.bottom * 40.f / 100.f;

	BOOL load_result = FALSE;
	ComPtr<IWICImagingFactory> p_wic_factory;
	ComPtr<IWICBitmapFrameDecode> p_frame;
	ComPtr<IWICBitmapDecoder> p_decoder;
	ComPtr<IWICFormatConverter> p_converter;
	if (SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(p_wic_factory.GetAddressOf()))))
	{
		if (SUCCEEDED(p_wic_factory->CreateDecoderFromFilename(L"res/google.png", NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, p_decoder.GetAddressOf())))
		{
			if (SUCCEEDED(p_decoder->GetFrame(0, p_frame.GetAddressOf())))
			{
				if (SUCCEEDED(p_wic_factory->CreateFormatConverter(p_converter.GetAddressOf())))
				{
					if (SUCCEEDED(p_converter->Initialize(p_frame.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom)))
					{
						if (SUCCEEDED(gp_render_target->CreateBitmapFromWicBitmap(p_converter.Get(), NULL, gp_bitmap.GetAddressOf())))
						{
							load_result = TRUE;
						}
						else {
							MessageBox(NULL, L"CreateBitmapFromWicBitmap", L"ERROR", MB_OK);
						}
					}
					else {
						MessageBox(NULL, L"Initialize", L"ERROR", MB_OK);
					}
				}
				else {
					MessageBox(NULL, L"CreateFormatConverter", L"ERROR", MB_OK);
				}
			}
			else {
				MessageBox(NULL, L"GetFrame", L"ERROR", MB_OK);
			}
		}
		else {
			MessageBox(NULL, L"CreateDecoderFromFilename", L"ERROR", MB_OK);
		}
	}
	else {
		MessageBox(NULL, L"CoCreateInstance", L"ERROR", MB_OK);
	}
	
	
	// WIC 객체 해제
	p_converter.Reset();
	p_frame.Reset();
	p_decoder.Reset();
	p_wic_factory.Reset();

	// 에디트 컨트롤1 생성
	int control_left = 0;
	int control_top = 0;
	int control_width = 0;
	int control_height = 0;
	control_left = wnd_rect.right / 3.f;
	control_top = wnd_rect.bottom * 55.f / 100.f;
	control_width = wnd_rect.right * 2.f/3.f - control_left;
	control_height = wnd_rect.bottom * 60.f / 100.f - control_top;
	//g_hEdit1 = CreateWindow(L"EDIT", L"", WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT,
	//	100, 100, 300, 180, g_hWnd, NULL, g_hInst, NULL);

	// 에디트 컨트롤2 생성
	control_left = wnd_rect.right / 3.f;
	control_top = wnd_rect.bottom * 65.f / 100.f;
	control_width = wnd_rect.right * 2.f / 3.f - control_left;
	control_height = wnd_rect.bottom * 70.f / 100.f - control_top;
	//g_hEdit2 = CreateWindow(L"EDIT", L"", WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT,
	//	control_left, control_top, control_width, control_height, g_hWnd, NULL, g_hInst, NULL);

	// 버튼 색상
	gp_render_target->CreateSolidColorBrush(ColorF(ColorF::CornflowerBlue), gp_buttonNormalBrush.GetAddressOf());
	gp_render_target->CreateSolidColorBrush(ColorF(ColorF::AliceBlue), gp_buttonTextNormalBrush.GetAddressOf());
	gp_render_target->CreateSolidColorBrush(ColorF(ColorF::SteelBlue), gp_buttonPushedBrush.GetAddressOf());
	gp_render_target->CreateSolidColorBrush(ColorF(ColorF::AliceBlue), gp_buttonTextPushedBrush.GetAddressOf());

	// 에디트 컨트롤 색상
	gp_render_target->CreateSolidColorBrush(ColorF(ColorF::Black), gp_edit1TextBrush.GetAddressOf());
	gp_render_target->CreateSolidColorBrush(ColorF(ColorF::White), gp_edit1BackBrush.GetAddressOf());
	gp_render_target->CreateSolidColorBrush(ColorF(ColorF::Gray), gp_edit1BorderBrush.GetAddressOf());
	gp_render_target->CreateSolidColorBrush(ColorF(ColorF::Blue), gp_edit1BorderFocusedBrush.GetAddressOf());
	gp_render_target->CreateSolidColorBrush(ColorF(ColorF::Black), gp_edit1CursorBrush.GetAddressOf());

	gp_render_target->CreateSolidColorBrush(ColorF(ColorF::Black), gp_edit2TextBrush.GetAddressOf());
	gp_render_target->CreateSolidColorBrush(ColorF(ColorF::White), gp_edit2BackBrush.GetAddressOf());
	gp_render_target->CreateSolidColorBrush(ColorF(ColorF::Gray), gp_edit2BorderBrush.GetAddressOf());
	gp_render_target->CreateSolidColorBrush(ColorF(ColorF::Blue), gp_edit2BorderFocusedBrush.GetAddressOf());
	gp_render_target->CreateSolidColorBrush(ColorF(ColorF::Black), gp_edit2CursorBrush.GetAddressOf());

	// 배경 Pink, 글자 DeepPink, 배경 CornflowerBlue

	// DWriteFactory 생성 (추후 메인으로 이동)
	DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(gp_writeFactory.GetAddressOf()));

	// 버튼 텍스트 포맷 설정
	gp_writeFactory->CreateTextFormat(
		L"Malgun Gothic",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		20.0f,					// 글꼴 크기
		L"ko-kr",				// 로케일 (한국어)
		gp_buttonTextFormat.GetAddressOf()
	);
	gp_buttonTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	gp_buttonTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	// 에디트 텍스트 포맷 설정
	gp_writeFactory->CreateTextFormat(
		L"Malgun Gothic",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		14.0f,					// 글꼴 크기
		L"ko-kr",				// 로케일 (한국어)
		gp_edit1TextFormat.GetAddressOf()
	);
	gp_edit1TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	gp_edit1TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	gp_writeFactory->CreateTextFormat(
		L"Malgun Gothic",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		14.0f,					// 글꼴 크기
		L"ko-kr",				// 로케일 (한국어)
		gp_edit2TextFormat.GetAddressOf()
	);
	gp_edit2TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	gp_edit2TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	disableIMMContext();
}

void WndIME_COMPOSITION(LPARAM lParam) {
	HIMC hIMC = ImmGetContext(g_hWnd);
	if (hIMC) {
		if (lParam & GCS_COMPSTR) {  // 조합 중인 문자열이 있을 때
			DWORD dwSize = ImmGetCompositionString(hIMC, GCS_COMPSTR, NULL, 0);
			if (dwSize > 0) {
				wchar_t* compStr = new wchar_t[(dwSize / sizeof(wchar_t)) + 1];
				ImmGetCompositionString(hIMC, GCS_COMPSTR, compStr, dwSize);
				compStr[dwSize / sizeof(wchar_t)] = 0;
				wchar_t temp[256]{ 0 };

				if (g_isFocusedEdit1)
				{
					wcscpy(temp, wstr_edit1);
					int len = wcslen(wstr_edit1);
					temp[len] = compStr[0];
					wcscpy(wstr_edit1_view, temp);					// 원본을 복제하고
				}
				else if (g_isFocusedEdit2)
				{
					wcscpy(temp, wstr_edit2);
					int len = wcslen(wstr_edit2);
					temp[len] = compStr[0];
					wcscpy(wstr_edit2_view, temp);					// 원본을 복제하고
				}
				WndPAINT();

				//wprintf(L"%s", compStr);

				delete[] compStr;
			}
		}
		ImmReleaseContext(g_hWnd, hIMC);
	}
}


INT_PTR CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_PAINT:
		::ValidateRect(hWnd, NULL);				// WM_PAINT를 다시 발생하지 않게 한다
		WndPAINT();
		break;
	case WM_COMMAND:
		
		break;
	case WM_TIMER:
		
		break;
	case WM_MOUSEMOVE:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			WndMOUSEMOVE(x, y);
		}
		break;
	
	case WM_LBUTTONDOWN:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			WndLBUTTONDOWN(x, y);
		}
		break;
	case WM_LBUTTONUP:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			WndLBUTTONUP(x, y);
		}
		break;
	case WM_CHAR:
		WndCHAR(static_cast<wchar_t>(wParam));
		break;
	case WM_IME_COMPOSITION: {
		WndIME_COMPOSITION(lParam);
		break;
	}
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

	// D2D 라이브러리 초기화
	(void)CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	// D2D factory 생성
	if (S_OK != D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, gp_factory.GetAddressOf())) {
		return 0;
	}

	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = g_hInst;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = my_class_name;
	wcex.hIconSm = NULL;
	RegisterClassExW(&wcex);

	// 화면 중앙 위치 및 크기 계산
	int windowWidth = 1280;
	int windowHeight = 960;
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int posX = (screenWidth - windowWidth) / 2;
	int posY = (screenHeight - windowHeight) / 2;

	g_hWnd = CreateWindow(my_class_name, _T("로그인 화면"), WS_POPUP, posX, posY, windowWidth, windowHeight, NULL, NULL, hInstance, NULL);

	::RECT wnd_rect;
	::GetClientRect(g_hWnd, &wnd_rect);

	float control_left = wnd_rect.right / 3.f;
	float control_top = wnd_rect.bottom * 75.f / 100.f;
	float control_right = wnd_rect.right * 2.f / 3.f;
	float control_bottom = wnd_rect.bottom * 80.f / 100.f;
	g_buttonNormalRect = { control_left, control_top, control_right, control_bottom };
	g_buttonPushedRect = { control_left+1, control_top+1, control_right, control_bottom };
	g_buttonNormalRoundRect = { g_buttonNormalRect, 15.f, 15.f };
	g_buttonPushedRoundRect = { g_buttonPushedRect, 15.f, 15.f };

	control_top = wnd_rect.bottom * 65.f / 100.f;
	control_bottom = wnd_rect.bottom * 69.f / 100.f;
	g_edit1Rect = { control_left, control_top, control_right, control_bottom };
	g_edit1RoundRect = { g_edit1Rect , 15.f, 15.f};

	control_top = wnd_rect.bottom * 70.f / 100.f;
	control_bottom = wnd_rect.bottom * 74.f / 100.f;
	g_edit2Rect = { control_left, control_top, control_right, control_bottom };
	g_edit2RoundRect = { g_edit2Rect , 15.f, 15.f};

	// 모서리 둥글게 (WIN11)
	DWM_WINDOW_CORNER_PREFERENCE preference = DWM_WINDOW_CORNER_PREFERENCE::DWMWCP_ROUND;
	DwmSetWindowAttribute(g_hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

	ShowWindow(g_hWnd, SW_SHOW);
	UpdateWindow(g_hWnd);

	MSG msg = {};
	while (msg.message != WM_QUIT) 
	{
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			// 렌더링?
			WndPAINT();
		}
	}

	// 라이브러리 해제
	CoUninitialize();
	return (int)msg.wParam;
}