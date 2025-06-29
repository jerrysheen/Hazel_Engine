#pragma once

#include "Hazel/Core/Core.h"

namespace Hazel {

	// CRTP（Curiously Recurring Template Pattern）单例模式模板类
	template<typename T>
	class Singleton
	{
	protected:
		// 构造和析构函数设为保护的，防止外部直接创建实例
		Singleton() = default;
		virtual ~Singleton() = default;
		
		// 禁止拷贝和移动
		Singleton(const Singleton&) = delete;
		Singleton& operator=(const Singleton&) = delete;
		
	public:
		// 获取单例实例的接口
		static T& Get()
		{
			if (!s_Instance)
				s_Instance = CreateInstance();
			return *s_Instance;
		}
		
		// 获取原始指针（不转移所有权）
		static T* GetPtr()
		{
			return s_Instance ? s_Instance.get() : nullptr;
		}
		
		// 释放单例实例
		static void Release()
		{
			s_Instance.reset();
		}
		
	protected:
		// 可以被子类重写的创建实例方法，允许继承层次中的自定义实例化
		static Ref<T> CreateInstance()
		{
			return CreateRef<T>();
		}
		
	private:
		static Ref<T> s_Instance;
	};

	// 静态成员变量声明（定义在 Singleton.cpp 中）
    // 参考Singleton_doc.md,为什么要在这个地方初始化。
	template<typename T>
	Ref<T> Singleton<T>::s_Instance = nullptr;

} // namespace Hazel
