#pragma once

/// <summary>
/// ���������ȾAPI�ĳ����࣬��Opengl�У������ʱ�����ǲ�����Ҫ����
/// ��ȾAPI���������ִ�API����һ���Լ��ĳ�ʼ�����Լ����Ӿ�ϸ�Ĺ�������
/// ���Ǵ�������ʹ����Դ������Ҫ��API����н������ʴ���Ҫ���API��
/// ���磬��ʼ����ʱ��D3D12��Ҫ��ʼ��Device, CommandListAlloc, CommandList,
/// ����������Դʱ������Ҫͨ��Device�����д�����
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