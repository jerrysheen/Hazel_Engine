#include "hzpch.h"
#include "MaterialLibrary.h"
#include "Runtime/Graphics/Material/Material.h"
#include "Runtime/Graphics/Material/MaterialSerializer.h"

namespace Hazel 
{
	MaterialLibrary& MaterialLibrary::Get()
	{
		static MaterialLibrary instance;
		return instance;
	}

	// ✅ 主要材质加载入口 - 智能缓存系统
	Ref<Material> MaterialLibrary::LoadMaterial(const std::string& path)
	{
		// 1. 检查弱指针缓存
		auto pathIt = m_PathCache.find(path);
		if (pathIt != m_PathCache.end()) {
			if (auto material = pathIt->second.lock()) {
				HZ_CORE_TRACE("MaterialLibrary: Path cache hit for '{0}'", path);
				return material;
			}
			// 弱指针已失效，从缓存中移除
			m_PathCache.erase(pathIt);
		}

		// 2. 检查智能缓存（带文件修改时间检测）
		auto smartIt = m_SmartCache.find(path);
		if (smartIt != m_SmartCache.end()) {
			try {
				auto fileTime = std::filesystem::last_write_time(path);
				if (fileTime == smartIt->second.lastModified) {
					// 文件未修改，复用缓存
					auto material = smartIt->second.material;
					smartIt->second.lastAccess = std::chrono::steady_clock::now();
					
					// 重新添加到弱指针缓存
					m_PathCache[path] = material;
					
					HZ_CORE_TRACE("MaterialLibrary: Smart cache hit for '{0}'", path);
					return material;
				} else {
					// 文件已修改，移除过期缓存
					m_SmartCache.erase(smartIt);
					HZ_CORE_INFO("MaterialLibrary: File '{0}' modified, invalidating cache", path);
				}
			} catch (const std::filesystem::filesystem_error& e) {
				HZ_CORE_WARN("MaterialLibrary: Failed to check file time for '{0}': {1}", path, e.what());
				m_SmartCache.erase(smartIt);
			}
		}

		// 3. 从文件加载新材质
		Ref<Material> material = MaterialSerializer::DeserializeFromJSON(path);
		if (material) {
			// 设置材质ID
			material->SetMaterialID(path);
			
			// 添加到两级缓存
			m_PathCache[path] = material;
			
			try {
				auto fileTime = std::filesystem::last_write_time(path);
				m_SmartCache.emplace(path, SmartCacheEntry(material, fileTime));
			} catch (const std::filesystem::filesystem_error& e) {
				HZ_CORE_WARN("MaterialLibrary: Failed to get file time for '{0}': {1}", path, e.what());
			}
			
			HZ_CORE_INFO("MaterialLibrary: Loaded and cached material from '{0}'", path);
		} else {
			HZ_CORE_ERROR("MaterialLibrary: Failed to load material from '{0}'", path);
		}
		
		return material;
	}

	// ✅ 创建独立材质实例
	Ref<Material> MaterialLibrary::CreateUniqueMaterial(const std::string& path)
	{
		// 直接加载，不使用缓存
		Ref<Material> material = MaterialSerializer::DeserializeFromJSON(path);
		if (material) {
			// 创建唯一ID
			std::string uniqueId = path + "_unique_" + std::to_string(reinterpret_cast<uintptr_t>(material.get()));
			material->SetMaterialID(uniqueId);
			HZ_CORE_TRACE("MaterialLibrary: Created unique material from '{0}' with ID '{1}'", path, uniqueId);
		}
		return material;
	}

	void MaterialLibrary::Register(const std::string& name, const Ref<Material>& material)
	{
		if (material) {
			m_NamedMaterials[name] = material;
			HZ_CORE_TRACE("MaterialLibrary: Registered material '{0}'", name);
		} else {
			HZ_CORE_WARN("MaterialLibrary: Attempted to register null material '{0}'", name);
		}
	}

	Ref<Material> MaterialLibrary::Get(const std::string& name)
	{
		if (Exists(name)) {
			return m_NamedMaterials[name];
		}
		
		HZ_CORE_WARN("MaterialLibrary: Material '{0}' not found", name);
		return nullptr;
	}

	bool MaterialLibrary::Exists(const std::string& name) const
	{
		return m_NamedMaterials.find(name) != m_NamedMaterials.end();
	}

	bool MaterialLibrary::Save(const std::string& name, const std::string& filepath)
	{
		if (!Exists(name)) {
			HZ_CORE_ERROR("MaterialLibrary: Cannot save material '{0}' - not found in library", name);
			return false;
		}
		
		try {
			MaterialSerializer::SerializeToJSON(m_NamedMaterials[name], filepath);
			HZ_CORE_INFO("MaterialLibrary: Saved material '{0}' to '{1}'", name, filepath);
			return true;
		} catch (const std::exception& e) {
			HZ_CORE_ERROR("MaterialLibrary: Failed to save material '{0}': {1}", name, e.what());
			return false;
		}
	}

	Ref<Material> MaterialLibrary::Load(const std::string& filepath)
	{
		// 使用主加载方法，享受缓存优势
		return LoadMaterial(filepath);
	}

	Ref<Material> MaterialLibrary::LoadAndRegister(const std::string& name, const std::string& filepath)
	{
		Ref<Material> material = LoadMaterial(filepath);
		if (material) {
			Register(name, material);
			HZ_CORE_INFO("MaterialLibrary: Loaded and registered material '{0}' from '{1}'", name, filepath);
		}
		return material;
	}

	void MaterialLibrary::Unload(const std::string& name)
	{
		auto it = m_NamedMaterials.find(name);
		if (it != m_NamedMaterials.end()) {
			m_NamedMaterials.erase(it);
			HZ_CORE_TRACE("MaterialLibrary: Unloaded material '{0}'", name);
		} else {
			HZ_CORE_WARN("MaterialLibrary: Attempted to unload non-existent material '{0}'", name);
		}
	}

	// ✅ 缓存管理方法
	void MaterialLibrary::ClearCache()
	{
		size_t pathCacheCount = m_PathCache.size();
		size_t smartCacheCount = m_SmartCache.size();
		size_t namedCount = m_NamedMaterials.size();
		
		m_PathCache.clear();
		m_SmartCache.clear();
		m_NamedMaterials.clear();
		
		HZ_CORE_INFO("MaterialLibrary: Cleared all caches - Path: {0}, Smart: {1}, Named: {2}", 
			pathCacheCount, smartCacheCount, namedCount);
	}

	void MaterialLibrary::ClearPathCache()
	{
		size_t cleared = m_PathCache.size();
		m_PathCache.clear();
		HZ_CORE_INFO("MaterialLibrary: Cleared path cache - {0} entries", cleared);
	}

	void MaterialLibrary::ClearNamedMaterials()
	{
		size_t cleared = m_NamedMaterials.size();
		m_NamedMaterials.clear();
		HZ_CORE_INFO("MaterialLibrary: Cleared named materials - {0} entries", cleared);
	}

	size_t MaterialLibrary::GetCacheSize() const
	{
		return m_PathCache.size() + m_SmartCache.size() + m_NamedMaterials.size();
	}

	size_t MaterialLibrary::GetPathCacheSize() const
	{
		// 清理失效的弱指针并返回实际大小
		const_cast<MaterialLibrary*>(this)->CleanupExpiredEntries();
		return m_PathCache.size();
	}

	bool MaterialLibrary::IsCached(const std::string& path) const
	{
		// 检查弱指针缓存
		auto pathIt = m_PathCache.find(path);
		if (pathIt != m_PathCache.end() && !pathIt->second.expired()) {
			return true;
		}
		
		// 检查智能缓存
		auto smartIt = m_SmartCache.find(path);
		if (smartIt != m_SmartCache.end()) {
			return true;
		}
		
		return false;
	}

	std::vector<std::string> MaterialLibrary::GetAllMaterialNames() const
	{
		std::vector<std::string> names;
		names.reserve(m_NamedMaterials.size());
		
		for (const auto& [name, material] : m_NamedMaterials) {
			names.push_back(name);
		}
		
		return names;
	}

	void MaterialLibrary::PrintCacheInfo() const
	{
		size_t pathCacheValid = 0;
		for (const auto& [path, weakPtr] : m_PathCache) {
			if (!weakPtr.expired()) {
				pathCacheValid++;
			}
		}
		
		HZ_CORE_INFO("MaterialLibrary Cache Info:");
		HZ_CORE_INFO("  - Path Cache: {0} entries ({1} valid)", m_PathCache.size(), pathCacheValid);
		HZ_CORE_INFO("  - Smart Cache: {0} entries", m_SmartCache.size());
		HZ_CORE_INFO("  - Named Materials: {0} entries", m_NamedMaterials.size());
		HZ_CORE_INFO("  - Total Cache Size: {0} entries", GetCacheSize());
	}

	// 内部辅助方法
	void MaterialLibrary::CleanupExpiredEntries()
	{
		auto it = m_PathCache.begin();
		while (it != m_PathCache.end()) {
			if (it->second.expired()) {
				it = m_PathCache.erase(it);
			} else {
				++it;
			}
		}
	}

	bool MaterialLibrary::IsFileModified(const std::string& path, std::filesystem::file_time_type cachedTime) const
	{
		try {
			auto currentTime = std::filesystem::last_write_time(path);
			return currentTime != cachedTime;
		} catch (const std::filesystem::filesystem_error&) {
			return true; // 如果无法访问文件，认为已修改
		}
	}
} 