#pragma once

/// <summary>
/// 这个类是渲染API的抽象类，在Opengl中，大多数时候我们并不需要管理
/// 渲染API本身，但是现代API都有一套自己的初始化，以及更加精细的管理流程
/// 我们创建或者使用资源，都需要和API层进行交互，故此需要这个API。
/// 比如，初始化的时候D3D12需要初始化Device, CommandListAlloc, CommandList,
/// 后续创建资源时，又需要通过Device来进行创建。
/// </summary>
namespace Hazel {

	class HAZEL_API RenderAPIManager
	{
	public:
		virtual ~RenderAPIManager() {}
		static void Init();
		virtual void OnUpdate() {};
		static RenderAPIManager* Create();
	};
}