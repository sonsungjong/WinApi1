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
#include <chrono>
#include <condition_variable>
#include <unordered_map>
#include <string>
#include <vector>

#include <WinSock2.h>
#include <WS2tcpip.h>
#include "framework.h"

#include <d2d1.h>						// Direct2D 기본 헤더
#include <dwrite.h>						// DirectWrite 헤더 (텍스트 렌더링이 필요한 경우)
#include <wincodec.h>					 // WIC 헤더 (이미지 로딩을 위한 필수 요소)
#include <wrl.h>

#include <dwmapi.h>
#include <imm.h>


#pragma comment(lib, "d2d1.lib")						// Direct2D 라이브러리
#pragma comment(lib, "dwrite.lib")						// DirectWrite 라이브러리
#pragma comment(lib, "windowscodecs.lib")		 // WIC 라이브러리 (이미지 디코딩 필수)
#pragma comment(lib, "Dwmapi.lib")
#pragma comment(lib, "imm32.lib")


#endif //PCH_H
