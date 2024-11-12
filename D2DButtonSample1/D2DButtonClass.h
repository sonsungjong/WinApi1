#pragma once

struct ST_Button
{
    float left_x;
    float top_y;
    float right_x;
    float bottom_y;
    int isPressed;
    wchar_t text[256];
    std::function<void()> callback;
};

class D2DButtonClass {
public:
    D2DButtonClass(HWND _hWnd, LPRECT _rect);                 // 부모 윈도우 핸들과 D2D 출력 영역을 받아온다
    ~D2DButtonClass();
    

    void initD2D(HWND _hWnd);                 // D2D 초기화 진행, 팩토리를 생성하고 랜더타겟을 설정한다
    HWND m_hWnd;                        // 부모 윈도우 핸들, 해당 윈도우에 버튼을 그린다

    std::unordered_map<int, ST_Button> m_umapButton;
    void createButton(int id, ST_Button btn);                   // 버튼을 만든다

    
    void UpdateView();
    void OnDraw();
    void SetClickCallback(int id, std::function<void()> callback);          // 해당 id의 버튼 클릭 이벤트를 변경한다
    void OnClick(int id);                       // 해당 id의 버튼 콜백 함수를 실행한다

    ID2D1HwndRenderTarget* m_renderTarget;
    ID2D1SolidColorBrush* m_brush;
    IDWriteTextFormat* m_textFormat;
};
