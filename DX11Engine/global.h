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
#include <string>
#include <typeinfo>

#include "define.h"
#include "enum.h"
#include "struct.h"
#include "func.h"
#include "singleton.h"
#include "ptr.h"