// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <string>
#include <vector>

//#include <WinSock2.h>
//#include <WS2tcpip.h>
#include "framework.h"

#include <d2d1.h>						// Direct2D �⺻ ���
#include <dwrite.h>						// DirectWrite ��� (�ؽ�Ʈ �������� �ʿ��� ���)
#include <wincodec.h>					 // WIC ��� (�̹��� �ε��� ���� �ʼ� ���)
#include <wrl.h>

#pragma comment(lib, "d2d1.lib")						// Direct2D ���̺귯��
#pragma comment(lib, "dwrite.lib")						// DirectWrite ���̺귯��
#pragma comment(lib, "windowscodecs.lib")		 // WIC ���̺귯�� (�̹��� ���ڵ� �ʼ�)


#endif //PCH_H
