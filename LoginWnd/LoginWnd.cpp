#include "pch.h"
#include "framework.h"
#include "LoginWnd.h"

using namespace D2D1;
using namespace Microsoft::WRL;

HINSTANCE g_hInst;
HWND g_hWnd;

ComPtr<ID2D1Factory> gp_factory;						// D2D ��ü ���� ���丮 (���μ��� �ϳ�)
ComPtr<IDWriteFactory> gp_writeFactory;				// DWrite ��ü (���μ��� �ϳ�)


// �� wnd���� �����Ǿ��ϴ� ��ü
// D2D Ŭ���̾�Ʈ�� �׸��� �׸��� ��ü
ComPtr<ID2D1HwndRenderTarget> gp_render_target;

// �̹����� �ø� ����
ComPtr<ID2D1Bitmap> gp_bitmap;
D2D1_RECT_F g_img_rect;


// ��ư
ComPtr<ID2D1SolidColorBrush> gp_buttonNormalBrush;
ComPtr<ID2D1SolidColorBrush> gp_buttonPushedBrush;
ComPtr<ID2D1SolidColorBrush> gp_buttonTextNormalBrush;
ComPtr<ID2D1SolidColorBrush> gp_buttonTextPushedBrush;
ComPtr<IDWriteTextFormat> gp_buttonTextFormat;					// ��ư

D2D1_RECT_F g_buttonNormalRect = {  };
D2D1_RECT_F g_buttonPushedRect = {  };
D2D1_ROUNDED_RECT g_buttonNormalRoundRect = {  };			// �������
D2D1_ROUNDED_RECT g_buttonPushedRoundRect = {  };			// �������� ��¦ �����ϴ����� �̵��ؼ� ���� ������ �ش�
bool g_isButtonPushed = false;

// ����Ʈ ��Ʈ�� ����
bool g_isKoreanMode = false;

// ����Ʈ ��Ʈ��1
ComPtr<IDWriteTextFormat> gp_edit1TextFormat;					// ����Ʈ ��Ʈ��
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

// ����Ʈ ��Ʈ��2
ComPtr<IDWriteTextFormat> gp_edit2TextFormat;					// ����Ʈ ��Ʈ��
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
	const wchar_t* buttonText = L"�α���";

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
	// ���� �׵θ�
	//gp_render_target->FillRectangle(g_cursorPos, gp_editCursorBrush.Get());				// Ŀ��

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
				WndPAINT();				// �ؽ�Ʈ�� ȭ�鿡 �ݿ�
			}
				break;
		default:
			if (nLen < 255) {
				wstr_edit1[nLen] = ch;
				wcscpy(wstr_edit1_view, wstr_edit1);
				gp_render_target->DrawText(wstr_edit1_view, wcslen(wstr_edit1_view), gp_edit1TextFormat.Get(), g_edit1Rect, gp_edit1TextBrush.Get());
				WndPAINT();				// �ؽ�Ʈ�� ȭ�鿡 �ݿ�
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
				WndPAINT();				// �ؽ�Ʈ�� ȭ�鿡 �ݿ�
			}
			break;
		default:
			if (nLen < 255) {
				wstr_edit2[nLen] = ch;
				wcscpy(wstr_edit2_view, wstr_edit2);
				gp_render_target->DrawText(wstr_edit2_view, wcslen(wstr_edit2_view), gp_edit2TextFormat.Get(), g_edit2Rect, gp_edit2TextBrush.Get());
				WndPAINT();				// �ؽ�Ʈ�� ȭ�鿡 �ݿ�
			}
			break;
		}
		edit1_dump = L'\0';
	}
}

// �ѱ۹��UI ������
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

	// �̹����� ������ �̹����� ����� ����
	// �̹����� ������ Clear�Ѵ�
	if (gp_bitmap.Get() != NULL) {
		gp_render_target->Clear(ColorF(1.0f, 1.0f, 1.0f));						// �Ͼ������ �����
		gp_render_target->DrawBitmap(gp_bitmap.Get(), &g_img_rect);			// ��Ʈ�ʿ� �̹������ڸ� �׸���
	}
	else {
		gp_render_target->Clear(ColorF(1.0f, 1.0f, 1.0f));						// �Ͼ������ �����
	}

	// ��ϵ� ��� ��ư �׸���
	DrawButton();

	// ��ϵ� ��� ����Ʈ ��Ʈ�� �׸���
	DrawEditControl();

	gp_render_target->EndDraw();
}

void WndLBUTTONDOWN(int x, int y)
{
	// ������ ��ư�� �ε����� ���ÿ��� id�� ����ϰ�
	// ������ ��ư�� �÷��׸� �ٲٰ�
	// �ش� ������ �ٽ� �׷������� ���Ž�Ű��
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

// ���ÿ��� �ε����� �ش��ϴ� ��ư�� �ִ��� üũ�ϰ�
// �ش� ��ư�� �÷��װ� true�̰� ��ư�� �����ȿ� ������
// ����� �ݹ��Լ��� ���۽�Ų�� (�ݹ���������, PostMessage�� ���� ����)
void WndLBUTTONUP(int x, int y) {
	if (g_isButtonPushed == true && x >= g_buttonNormalRect.left && x <= g_buttonNormalRect.right && y >= g_buttonNormalRect.top && y <= g_buttonNormalRect.bottom) {
		MessageBox(g_hWnd, L"��ư ����", L"��ư", MB_OK);		// ����id ��ư�� ���� �Լ��ߵ�
	}
	g_isButtonPushed = false;			// ���� ��ư�� ���� Ŭ���÷��� false ó��
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
	::RECT wnd_rect;
	::GetClientRect(g_hWnd, &wnd_rect);			// ������ ��ǥ ��ü�� rect�� �ݿ���

	// wnd_rect�� ����Ÿ���� �������� ���Ѵ�
	gp_factory->CreateHwndRenderTarget(RenderTargetProperties(), HwndRenderTargetProperties(g_hWnd, SizeU(wnd_rect.right, wnd_rect.bottom)), gp_render_target.GetAddressOf());
	D2D1_RECT_F excludeRect = D2D1::RectF(100, 100, 300, 180); // Edit ��Ʈ�� ����

	gp_render_target->BeginDraw();
	gp_render_target->PushAxisAlignedClip(excludeRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	gp_render_target->Clear(ColorF(1.0f, 1.0f, 1.0f)); // ������� Clear
	gp_render_target->PopAxisAlignedClip();
	gp_render_target->EndDraw();

	// �̹������� ũ�⸦ ���Ѵ�
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
	
	
	// WIC ��ü ����
	p_converter.Reset();
	p_frame.Reset();
	p_decoder.Reset();
	p_wic_factory.Reset();

	// ����Ʈ ��Ʈ��1 ����
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

	// ����Ʈ ��Ʈ��2 ����
	control_left = wnd_rect.right / 3.f;
	control_top = wnd_rect.bottom * 65.f / 100.f;
	control_width = wnd_rect.right * 2.f / 3.f - control_left;
	control_height = wnd_rect.bottom * 70.f / 100.f - control_top;
	//g_hEdit2 = CreateWindow(L"EDIT", L"", WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT,
	//	control_left, control_top, control_width, control_height, g_hWnd, NULL, g_hInst, NULL);

	// ��ư ����
	gp_render_target->CreateSolidColorBrush(ColorF(ColorF::CornflowerBlue), gp_buttonNormalBrush.GetAddressOf());
	gp_render_target->CreateSolidColorBrush(ColorF(ColorF::AliceBlue), gp_buttonTextNormalBrush.GetAddressOf());
	gp_render_target->CreateSolidColorBrush(ColorF(ColorF::SteelBlue), gp_buttonPushedBrush.GetAddressOf());
	gp_render_target->CreateSolidColorBrush(ColorF(ColorF::AliceBlue), gp_buttonTextPushedBrush.GetAddressOf());

	// ����Ʈ ��Ʈ�� ����
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

	// ��� Pink, ���� DeepPink, ��� CornflowerBlue

	// DWriteFactory ���� (���� �������� �̵�)
	DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(gp_writeFactory.GetAddressOf()));

	// ��ư �ؽ�Ʈ ���� ����
	gp_writeFactory->CreateTextFormat(
		L"Malgun Gothic",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		20.0f,					// �۲� ũ��
		L"ko-kr",				// ������ (�ѱ���)
		gp_buttonTextFormat.GetAddressOf()
	);
	gp_buttonTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	gp_buttonTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	// ����Ʈ �ؽ�Ʈ ���� ����
	gp_writeFactory->CreateTextFormat(
		L"Malgun Gothic",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		14.0f,					// �۲� ũ��
		L"ko-kr",				// ������ (�ѱ���)
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
		14.0f,					// �۲� ũ��
		L"ko-kr",				// ������ (�ѱ���)
		gp_edit2TextFormat.GetAddressOf()
	);
	gp_edit2TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	gp_edit2TextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	disableIMMContext();
}

void WndIME_COMPOSITION(LPARAM lParam) {
	HIMC hIMC = ImmGetContext(g_hWnd);
	if (hIMC) {
		if (lParam & GCS_COMPSTR) {  // ���� ���� ���ڿ��� ���� ��
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
					wcscpy(wstr_edit1_view, temp);					// ������ �����ϰ�
				}
				else if (g_isFocusedEdit2)
				{
					wcscpy(temp, wstr_edit2);
					int len = wcslen(wstr_edit2);
					temp[len] = compStr[0];
					wcscpy(wstr_edit2_view, temp);					// ������ �����ϰ�
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
		::ValidateRect(hWnd, NULL);				// WM_PAINT�� �ٽ� �߻����� �ʰ� �Ѵ�
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

	// D2D ���̺귯�� �ʱ�ȭ
	(void)CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	// D2D factory ����
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

	// ȭ�� �߾� ��ġ �� ũ�� ���
	int windowWidth = 1280;
	int windowHeight = 960;
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int posX = (screenWidth - windowWidth) / 2;
	int posY = (screenHeight - windowHeight) / 2;

	g_hWnd = CreateWindow(my_class_name, _T("�α��� ȭ��"), WS_POPUP, posX, posY, windowWidth, windowHeight, NULL, NULL, hInstance, NULL);

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

	// �𼭸� �ձ۰� (WIN11)
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
			// ������?
			WndPAINT();
		}
	}

	// ���̺귯�� ����
	CoUninitialize();
	return (int)msg.wParam;
}