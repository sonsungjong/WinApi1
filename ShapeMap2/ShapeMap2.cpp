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
	// ToDo : 초기화 작업

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


// 생성자
MainWindow::MainWindow()
{
}

// 소멸자
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
	RECT r;
	GetClientRect(m_hWnd, &r);

	long width = r.right - r.left;
	long height = r.bottom - r.top;

	m_dcp.CreateDCP(r.right - r.left, r.bottom - r.top);
	m_dcp.DCPTextSetting(24.0f, _T("맑은 고딕"), RGB24(0, 150, 150));
	m_dcp.mp_pen->SetColor(Gdiplus::Color(255, 0, 0, 255));
	m_dcp.SetPenThickness(2);
	m_dcp.Clear(RGB24(50, 70, 90));

	ShapeConvertClass* p_shp = new ShapeConvertClass;
	std::string strShapeDirectoryPath = "C:\\kmap_20230729";
	p_shp->readShapeDirectory(strShapeDirectoryPath);
	auto points = p_shp->getPoints();
	auto polylines = p_shp->getPolyLines();
	auto polygons = p_shp->getPolygon();

	// 지도 데이터의 경계를 계산
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

	// 지도 가로/세로 비율 계산
	float mapWidth = maxX - minX;
	float mapHeight = maxY - minY;

	// 윈도우 크기에 맞게 비율 조정
	float scaleX = static_cast<float>(width) / mapWidth;
	float scaleY = static_cast<float>(height) / mapHeight;
	float scale = std::min(scaleX, scaleY);  // 가로/세로 비율을 맞추기 위해 최소 스케일 사용
	int i = 0;
	for (const auto& polygon : polygons) 
	{
		const std::vector<std::pair<float, float>>& vertices = polygon.second;
		if (vertices.size() > 1) {
			Gdiplus::PointF* pointsArray = new Gdiplus::PointF[vertices.size()];
			for (size_t i = 0; i < vertices.size(); ++i) {
				//pointsArray[i].X = (vertices[i].first - minX) * scale;
				//pointsArray[i].Y = height - ((vertices[i].second - minY) * scale);
				pointsArray[i].X = (vertices[i].first - minX) * scale;
				pointsArray[i].Y = height - ((vertices[i].second - minY) * scale);
			}
			m_dcp.FillPolygon(pointsArray, static_cast<int>(vertices.size()), RGB24(0, 128, 178));
			delete[] pointsArray;
		}

	}

	//m_dcp.mp_pen->SetColor(Gdiplus::Color(255, 0, 0));
	//// 폴리라인을 그립니다.
	//for (const auto& polyline : polylines)
	//{
	//	const std::vector<std::pair<float, float>>& vertices = polyline.second;
	//	if (vertices.size() > 1) {
	//		Gdiplus::PointF* pointsArray = new Gdiplus::PointF[vertices.size()];
	//		for (size_t i = 0; i < vertices.size(); ++i) {
	//			pointsArray[i].X = (vertices[i].first - minX) * scale;
	//			pointsArray[i].Y = height - ((vertices[i].second - minY) * scale);
	//		}
	//		m_dcp.mp_graphic->DrawLines(m_dcp.mp_pen,pointsArray, vertices.size());
	//		delete[] pointsArray;
	//	}
	//}

	//for (const auto& point : points)
	//{
	//	const std::vector<std::pair<float, float>>& vertices = point.second;
	//	for (const auto& vertex : vertices) {
	//		float x = (vertex.first - minX) * scale;
	//		float y = height - ((vertex.second - minY) * scale);
	//		// 작은 원으로 점을 그립니다. (반지름 3)
	//		m_dcp.FillSolidEllipse(static_cast<int>(x) - 3, static_cast<int>(y) - 3, static_cast<int>(x) + 3, static_cast<int>(y) + 3, RGB24(255, 0, 0));
	//	}
	//}

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

	m_dcp.DCPText(0, 0, _T("안녕하세오"), RGB24(255,0,0));
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
