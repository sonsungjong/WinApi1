#include "pch.h"
#include "DCP.h"
#include "ShapeMap2.h"
#include "ShapeConvertClass.h"
#include <cmath>
#include <algorithm>
#include <limits>

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	MainWindow win;
	win.m_hInst = hInstance;

	if (!win.Create(L"ShapeMap2 Sample", WS_OVERLAPPEDWINDOW))
	{
		return 0;
	}
	// ToDo : �ʱ�ȭ �۾�

	ShowWindow(win.Window(), nCmdShow);
	UpdateWindow(win.Window());

	MSG msg = {};
	while (GetMessageW(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return (int)msg.wParam;
}


// ������
MainWindow::MainWindow()
{
}

// �Ҹ���
MainWindow::~MainWindow()
{
}

INT_PTR MainWindow::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		HANDLE_MSG(m_hWnd, WM_PAINT, OnPaint);
		HANDLE_MSG(m_hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(m_hWnd, WM_DESTROY, OnDestroy);
	case WM_COMMAND:
		OnCommand(wParam);
		break;
	}

	return DefWindowProc(m_hWnd, msg, wParam, lParam);
}

BOOL MainWindow::OnCreate(HWND ah_wnd, LPCREATESTRUCT lpCreateStruct)
{
	::RECT wnd_rect;
	::GetClientRect(m_hWnd, &wnd_rect);

	long wnd_width = wnd_rect.right - wnd_rect.left;
	long wnd_height = wnd_rect.bottom - wnd_rect.top;

	m_dcp.CreateDCP(wnd_rect.right - wnd_rect.left, wnd_rect.bottom - wnd_rect.top);
	m_dcp.DCPTextSetting(24.0f, _T("���� ���"), RGB24(0, 150, 150));
	m_dcp.mp_pen->SetColor(Gdiplus::Color(255, 0, 0, 255));
	m_dcp.SetPenThickness(2);
	m_dcp.Clear(RGB24(50, 70, 90));

	ShapeConvertClass* p_shp = new ShapeConvertClass;
	std::string strShapeDirectoryPath = "C:\\world_m2023_english_shp";
	p_shp->readShapeDirectory(strShapeDirectoryPath);
	auto points = p_shp->getPoints();
	auto polylines = p_shp->getPolyLines();
	auto polygons = p_shp->getPolygon();

	// ���� �������� ��踦 ���
	float minX = std::numeric_limits<float>::max();
	float minY = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::lowest();
	float maxY = std::numeric_limits<float>::lowest();

	for (const auto& polygon : polygons)
	{
		const std::vector<std::pair<float, float>>& vertices = polygon.second;
		for (const auto& vertex : vertices) {
			minX = std::min(minX, vertex.first);
			minY = std::min(minY, vertex.second);
			maxX = std::max(maxX, vertex.first);
			maxY = std::max(maxY, vertex.second);
		}
	}

	for (const auto& polyline : polylines)
	{
		const std::vector<std::pair<float, float>>& vertices = polyline.second;
		for (const auto& vertex : vertices) {
			minX = std::min(minX, vertex.first);
			minY = std::min(minY, vertex.second);
			maxX = std::max(maxX, vertex.first);
			maxY = std::max(maxY, vertex.second);
		}
	}

	for (const auto& point : points)
	{
		const std::vector<std::pair<float, float>>& vertices = point.second;
		for (const auto& vertex : vertices) {
			minX = std::min(minX, vertex.first);
			minY = std::min(minY, vertex.second);
			maxX = std::max(maxX, vertex.first);
			maxY = std::max(maxY, vertex.second);
		}
	}

	// ���� ����/���� ���� ���
	float map_width = maxX - minX;
	float map_height = maxY - minY;

	// ������ ũ�⿡ �°� ���� ����
	float scaleX = static_cast<float>(wnd_width) / map_width;
	float scaleY = static_cast<float>(wnd_height) / map_height;
	float scale = std::min(scaleX, scaleY);										// ����/���� ������ ���߱� ���� �ּ� ������ ���
	int k = 0;
	for (const auto& polygon : polygons) 
	{
		const std::vector<std::pair<float, float>>& vertices = polygon.second;
		if (vertices.size() > 0) {
			std::vector<Gdiplus::PointF> pointsArray(vertices.size());
			for (size_t i = 0; i < vertices.size(); ++i) {
				pointsArray[i].X = (vertices[i].first - minX) * scale;
				pointsArray[i].Y = wnd_height - ((vertices[i].second - minY) * scale);
			}
			m_dcp.DrawPolygon(pointsArray.data(), static_cast<int>(vertices.size()), RGB24(128, 128, 178));
			m_dcp.FillPolygon(pointsArray.data(), static_cast<int>(vertices.size()), RGB24(0, 128, 178));
		}
	}

	// ���������� �׸��ϴ�.
	m_dcp.mp_pen->SetColor(Gdiplus::Color(158, 158, 0));				// �����
	for (const auto& polyline : polylines)
	{
		const std::vector<std::pair<float, float>>& vertices = polyline.second;
		if (vertices.size() > 0) {
			//Gdiplus::PointF* pointsArray = new Gdiplus::PointF[vertices.size()];					// �����Ҵ翡�� ���ͷ� ����
			std::vector<Gdiplus::PointF> pointsArray(vertices.size());
			for (size_t i = 0; i < vertices.size(); ++i) {
				pointsArray[i].X = (vertices[i].first - minX) * scale;
				pointsArray[i].Y = wnd_height - ((vertices[i].second - minY) * scale);
			}
			m_dcp.mp_graphic->DrawLines(m_dcp.mp_pen, pointsArray.data(), static_cast<int>(vertices.size()));
		}
	}

	for (const auto& point : points)
	{
		const std::vector<std::pair<float, float>>& vertices = point.second;
		for (const auto& vertex : vertices) {
			float x = (vertex.first - minX) * scale;
			float y = wnd_height - ((vertex.second - minY) * scale);
			// ���� ������ ���� �׸��ϴ�. (������ 3)
			m_dcp.FillSolidEllipse(static_cast<int>(x) - 3, static_cast<int>(y) - 3, static_cast<int>(x) + 3, static_cast<int>(y) + 3, RGB24(0, 255, 0));					// �ʷ���
		}
	}

	if (p_shp != nullptr) {
		delete p_shp;
	}

	return TRUE;
}

void MainWindow::OnPaint(HWND ah_wnd)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(m_hWnd, &ps);

	m_dcp.FillSolidEllipse(100, 100, 200, 200, RGB24(0, 255, 0));
	m_dcp.FillSolidEllipse(150, 150, 250, 250);

	m_dcp.DCPText(0, 0, _T("�ȳ��ϼ���"), RGB24(0,0,0));
	m_dcp.DCPText(0, 100, _T("hello world!"));

	m_dcp.Draw(hdc);
	EndPaint(m_hWnd, &ps);
}



void MainWindow::OnDestroy(HWND ah_wnd)
{
	PostQuitMessage(0);
}

void MainWindow::OnCommand(WPARAM wParam)
{
	int wm_id = LOWORD(wParam);
}
