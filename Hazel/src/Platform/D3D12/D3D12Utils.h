#pragma once

#include <string>
#include <sstream>
#include <comdef.h>
#include <d3d12.h>

namespace Hazel {
    namespace D3D12Utils {
        
        // HRESULT转换为字符串的工具函数
        inline std::string HRESULTToString(HRESULT hr) {
            _com_error err(hr);
            std::stringstream ss;
            ss << "HRESULT: 0x" << std::hex << hr << " - " << err.ErrorMessage();
            return ss.str();
        }

        // 设置D3D12对象调试名称的工具函数
        inline void SetDebugName(ID3D12Object* obj, const char* name) {
            if (obj && name) {
                std::wstring wname(name, name + strlen(name));
                obj->SetName(wname.c_str());
            }
        }

        // 设置D3D12对象调试名称的工具函数 - 支持宽字符版本
        inline void SetDebugName(ID3D12Object* obj, const std::wstring& name) {
            if (obj) {
                obj->SetName(name.c_str());
            }
        }

    } // namespace D3D12Utils
} // namespace Hazel 