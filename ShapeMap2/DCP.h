#ifndef DCP_H_
#define DCP_H_

#include <gdiplus.h>

#define RGB24(r,g,b) (0xFF000000 | ((r) << 16) | ((g) << 8) | (b))
#define RGB32(a,r,g,b) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))

// GDI+, ������۸�
class DCP
{
protected:
	static ULONG_PTR m_token;				// GDI+ ���̺귯�� ��� ����
	static int m_object_count;					// DCP ��ü ���� ���

	Gdiplus::Bitmap* mp_bitmap;

	int m_last_position_x;
	int m_last_position_y;
	int m_width;				// ��� ��
	int m_height;				// ��� ����
	
public:
	DCP();
	DCP(int a_width, int a_height);
	virtual ~DCP();
	Gdiplus::Graphics* mp_graphic;
	DCP(const DCP& other) = delete;										// ���� ������
	DCP& operator=(const DCP& other) = delete;					// ���� ���� ������
	DCP(DCP&& other) noexcept = delete;								// �̵� ������
	DCP& operator=(DCP&& other) noexcept = delete;			// �̵� ���� ������

	Gdiplus::Brush* mp_brush;
	Gdiplus::Brush* mp_font_brush;
	Gdiplus::Pen* mp_pen = NULL;
	Gdiplus::Font* mp_font;
	Gdiplus::Color m_temp_color;				// ���� ������ ���� �ӽ� ��ü
	Gdiplus::Color m_prev_brush_color;				// ���� ������ ���� �ӽ� ��ü(�귯�ÿ�)

	void Init();
	void CreateDCP(int a_width, int a_height);
	void Draw(HDC ah_dc, int a_x = 0, int a_y = 0);				// DC�� ��� �ݿ�!! (�߿�)

	void Clear(Gdiplus::ARGB a_color = NULL);						// ��ü ���� ���� ä���
	
	// �簢�� (���� ä���)
	void Rectangle(int a_sx, int a_sy, int a_ex, int a_ey, Gdiplus::ARGB a_pen_color = NULL, Gdiplus::ARGB a_fill_color = NULL);
	void DrawRect(int a_sx, int a_sy, int a_ex, int a_ey, Gdiplus::ARGB a_pen_color = NULL);
	void FillSolidRect(int a_sx, int a_sy, int a_ex, int a_ey, Gdiplus::ARGB a_fill_color = NULL);

	// Ÿ�� (���� ä���)
	void Ellipse(int a_sx, int a_sy, int a_ex, int a_ey, Gdiplus::ARGB a_pen_color = NULL, Gdiplus::ARGB a_fill_color = NULL);
	void DrawEllipse(int a_sx, int a_sy, int a_ex, int a_ey, Gdiplus::ARGB a_pen_color = NULL);
	void FillSolidEllipse(int a_sx, int a_sy, int a_ex, int a_ey, Gdiplus::ARGB a_fill_color = NULL);

	// �� �׸���
	void DrawLine(int a_sx, int a_sy, int a_ex, int a_ey, Gdiplus::ARGB a_pen_color = NULL);
	void MoveTo(int a_sx, int a_sy);
	void LineTo(int a_ex, int a_ey, Gdiplus::ARGB a_pen_color = NULL);

	// �� ���� (�� ��Ÿ�ϰ� �β�)
	void SetPenStyle(Gdiplus::DashStyle a_style, Gdiplus::REAL a_thickness);
	void SetPenThickness(float a_thickness = -1.0f);

	// �ؽ�Ʈ ���
	void DCPText(int a_x, int a_y, const TCHAR* ap_str, Gdiplus::ARGB a_text_color = NULL);

	// �ؽ�Ʈ �۲ð� ũ�� ����, �ۻ���
	void DCPTextSetting(float a_font_size = NULL, const TCHAR* ap_font_style = NULL, Gdiplus::ARGB a_text_color = NULL);

	// �̹��� ��� (��θ� �޾� �ش� �̹��� ��� - ũ��� ���� �Է� ���ϸ� ����ũ�� ����)
	void DCPImage(const TCHAR* a_file_path, int a_x, int a_y, int a_width = 0, int a_height = 0);

	// �ٰ��� �׸���(�׵θ�)
	void FillPolygon(Gdiplus::Point* ap_position_list, int a_count, Gdiplus::ARGB a_fill_color = NULL);
	void FillPolygon(Gdiplus::PointF* ap_position_list, int a_count, Gdiplus::ARGB a_fill_color = NULL);

	// �ٰ��� �׸���(ä���)
	void DrawPolygon(Gdiplus::Point* ap_position_list, int a_count, Gdiplus::ARGB a_pen_color = NULL);
	void DrawPolygon(Gdiplus::PointF* ap_position_list, int a_count, Gdiplus::ARGB a_pen_color = NULL);

	// �ٰ��� �׸��� (��+ä���)
	void Polygon(Gdiplus::Point* ap_position_list, int a_count, Gdiplus::ARGB a_pen_color = NULL, Gdiplus::ARGB a_fill_color = NULL);
	void Polygon(Gdiplus::PointF* ap_position_list, int a_count, Gdiplus::ARGB a_pen_color = NULL, Gdiplus::ARGB a_fill_color = NULL);

	// Arc�׸��� (�׵θ�)
	void DrawArc(Gdiplus::REAL a_x, Gdiplus::REAL a_y, Gdiplus::REAL a_cx, Gdiplus::REAL a_cy, Gdiplus::REAL a_start_angle, Gdiplus::REAL a_sweep_angle, Gdiplus::ARGB a_pen_color = NULL);

	// �ձ� �簢��
	void DrawRoundRect(float a_x, float a_y, float a_width, float a_height, float a_radius, Gdiplus::ARGB a_pen_color = NULL);
	void FillRoundRect(float a_x, float a_y, float a_width, float a_height, float a_radius, Gdiplus::ARGB a_fill_color = NULL);

	// �׷��� �׸���

};

#endif			DCP_H_