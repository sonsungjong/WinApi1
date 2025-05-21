#ifndef PCH_H
#define PCH_H

#include <string>
#include <iostream>
#include <utility>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <future>
#include <memory>
#include <sstream>
#include <fstream>
#include <ostream>
#include <chrono>
#include <functional>
#include <ctime>
#include <stack>
#include <queue>
#include <cmath>
#include <algorithm>
#include <limits>
#include <filesystem>
#include <format>
#include <regex>

#include "framework.h"

// DX11
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")
// D2D
#pragma comment(lib, "d2d1.lib")
// GDI+
#pragma comment(lib, "gdiplus.lib")

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Comctl32.lib")

#endif //PCH_H
