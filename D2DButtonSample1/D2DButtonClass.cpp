#include "pch.h"
#include "D2DButtonClass.h"

D2DButton::D2DButton(HWND _hWnd, float x, float y, float width, float height, const std::wstring& text)
    : m_parent(_hWnd), m_x(x), m_y(y), m_width(width), m_height(height), m_text(text), m_isPressed(false) 
{
    m_hwnd = _hWnd;
}

D2DButton::~D2DButton() {}

void D2DButton::InitializeResources(ID2D1Factory* d2dFactory, IDWriteFactory* dwriteFactory) {
    d2dFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(m_hwnd, D2D1::SizeU(static_cast<UINT>(m_width), static_cast<UINT>(m_height))),
        &m_renderTarget
    );
    m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::SkyBlue), &m_brush);

    dwriteFactory->CreateTextFormat(
        L"Arial", nullptr, DWRITE_FONT_WEIGHT_BOLD,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
        24.0f, L"en-us", &m_textFormat
    );
}

void D2DButton::SetClickCallback(std::function<void()> callback) {
    m_clickCallback = callback;
}

void D2DButton::Draw() {
    if (!m_renderTarget) return;

    m_renderTarget->BeginDraw();
    m_renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

    // 버튼 색상 설정
    D2D1::ColorF buttonColor = m_isPressed ? D2D1::ColorF(D2D1::ColorF::SteelBlue) : D2D1::ColorF(D2D1::ColorF::SkyBlue);
    m_brush->SetColor(buttonColor);

    // 버튼 사각형 그리기
    m_renderTarget->FillRectangle(D2D1::RectF(20, 20, 50, 50), m_brush.Get());

    // 텍스트 색상 설정 및 텍스트 렌더링
    m_brush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
    m_renderTarget->DrawTextW(
        m_text.c_str(), m_text.size(),
        m_textFormat.Get(),
        D2D1::RectF(0, 0, m_width, m_height),
        m_brush.Get()
    );

    m_renderTarget->EndDraw();
}

void D2DButton::OnClick() {
    if (m_clickCallback) m_clickCallback();
}
