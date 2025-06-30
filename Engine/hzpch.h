#pragma once
// hazel precompiled header

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <array>
#include <cstdint>

#include <string>
#include <map>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "Runtime/Core/Log/Log.h"

#include <any>
#include <variant>
#include <stack>
#include <filesystem>
#include <mutex>
#include "Runtime/Debug/Instrumentor.h"

#include "boost/uuid/uuid.hpp"
#include "Runtime/Core/Utility/Unique.h"


#include "Platform/D3D12/d3dx12.h"
#include "Platform/D3D12/d3dUtil.h"

#ifdef HZ_PLATFORM_WINDOWS
	#include <Windows.h>
#endif


