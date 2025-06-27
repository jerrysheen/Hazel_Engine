#pragma once

#include "hzpch.h"
#include <string>
#include <unordered_map>
#include <chrono>
#include <filesystem>

namespace Hazel 
{
	class Material;

	// 材质库 - 负责材质资源的统一管理和缓存
	class MaterialLibrary {
	public:
		static MaterialLibrary& Get();
		
		// ✅ 主要加载入口 - 带智能缓存
		Ref<Material> LoadMaterial(const std::string& path);
		
		// ✅ 创建独立材质实例（不使用缓存）
		Ref<Material> CreateUniqueMaterial(const std::string& path);
		
		// 显式命名管理
		void Register(const std::string& name, const Ref<Material>& material);
		Ref<Material> Get(const std::string& name);
		bool Exists(const std::string& name) const;
		
		// 文件操作
		bool Save(const std::string& name, const std::string& filepath);
		Ref<Material> Load(const std::string& filepath);
		Ref<Material> LoadAndRegister(const std::string& name, const std::string& filepath);
		
		// 卸载材质
		void Unload(const std::string& name);
		
		// ✅ 缓存管理
		void ClearCache();
		void ClearPathCache();  // 只清理路径缓存
		void ClearNamedMaterials();  // 只清理命名材质
		size_t GetCacheSize() const;
		size_t GetPathCacheSize() const;
		bool IsCached(const std::string& path) const;
		
		// 获取所有已注册的材质名称
		std::vector<std::string> GetAllMaterialNames() const;
		
		// 调试和监控
		void PrintCacheInfo() const;
		
	private:
		MaterialLibrary() = default;
		
		// ✅ 分层缓存系统
		std::unordered_map<std::string, std::weak_ptr<Material>> m_PathCache;      // 路径缓存（弱引用）
		std::unordered_map<std::string, Ref<Material>> m_NamedMaterials;           // 命名材质（强引用）
		
		// ✅ 智能缓存条目（可选的高级缓存）
		struct SmartCacheEntry {
			Ref<Material> material;
			std::filesystem::file_time_type lastModified;
			std::chrono::steady_clock::time_point lastAccess;
			
			SmartCacheEntry(Ref<Material> mat, std::filesystem::file_time_type modified)
				: material(mat), lastModified(modified), lastAccess(std::chrono::steady_clock::now()) {}
		};
		std::unordered_map<std::string, SmartCacheEntry> m_SmartCache;
		
		// 内部辅助方法
		void CleanupExpiredEntries();
		bool IsFileModified(const std::string& path, std::filesystem::file_time_type cachedTime) const;
	};
} 