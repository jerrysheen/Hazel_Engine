#include "hzpch.h"
#include "Hazel/Graphics/RenderAPIManager.h"
#include "Platform/D3D12/D3D12RenderAPIManager.h"
//#include "Platform/Windows/WindowsDXGIWindow.h"

#include "Hazel/Graphics/RenderAPI.h"
namespace Hazel
{
	/// 初始化静态成员
	std::shared_ptr<RenderAPIManager> RenderAPIManager::instance = nullptr;
	std::mutex RenderAPIManager::mutex;
	std::function<Ref<RenderAPIManager>()> RenderAPIManager::createFunction = nullptr;

	//void RenderAPIManager::Init()
	//{
	//	switch (RenderAPI::GetAPI())
	//	{
	//		case RenderAPI::API::None: HZ_CORE_ASSERT(false, "RenderAPI::None is currently not supported"); break;
	//		//case RenderAPI::API::OpenGL: return new RenderAPIManager(props); break;
	//		case RenderAPI::API::DirectX12: instance = std::make_shared<D3D12RenderAPIManager>();
	//		  return;
	//	}
	//	instance = nullptr;
	//}
}
