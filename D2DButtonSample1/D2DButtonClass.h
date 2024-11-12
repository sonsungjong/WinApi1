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
    D2DButtonClass(HWND _hWnd, LPRECT _rect);                 // �θ� ������ �ڵ�� D2D ��� ������ �޾ƿ´�
    ~D2DButtonClass();
    

    void initD2D(HWND _hWnd);                 // D2D �ʱ�ȭ ����, ���丮�� �����ϰ� ����Ÿ���� �����Ѵ�
    HWND m_hWnd;                        // �θ� ������ �ڵ�, �ش� �����쿡 ��ư�� �׸���

    std::unordered_map<int, ST_Button> m_umapButton;
    void createButton(int id, ST_Button btn);                   // ��ư�� �����

    
    void UpdateView();
    void OnDraw();
    void SetClickCallback(int id, std::function<void()> callback);          // �ش� id�� ��ư Ŭ�� �̺�Ʈ�� �����Ѵ�
    void OnClick(int id);                       // �ش� id�� ��ư �ݹ� �Լ��� �����Ѵ�

    ID2D1HwndRenderTarget* m_renderTarget;
    ID2D1SolidColorBrush* m_brush;
    IDWriteTextFormat* m_textFormat;
};
