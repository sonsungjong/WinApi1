#pragma once

#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#include "SimpleMath.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace DirectX::SimpleMath;

#include <vector>
#include <list>
#include <map>
#include <unordered_map>

#include "define.h"
#include "struct.h"

#include "singleton.h"