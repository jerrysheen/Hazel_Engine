#pragma once
#include "CommandList.h"
#include <mutex>
namespace Hazel 
{
	class CommandPool 
	{
		// ����ط��������ɵ����ֻ࣬��Ҫ��RenderingAPI Manager ������ʼ����֮���ٳ�ʼ������ͺ��˰�

        private:
            // ˽�о�ָ̬�룬���ڴ洢���Ψһʵ��
            static Ref<CommandPool> instance;
            static std::mutex mutex;

            // �����캯����������������Ϊprotected��ֹ�ⲿ���������
            CommandPool() {}  // ���캯�������������ĳ�ʼ�����룩
            ~CommandPool() {}  // ��������
            // ˽���࣬�������� make_shared
            struct CommandPoolEnabler : public CommandPool {};
        public:
            // ɾ���������캯���͸�ֵ��������ȷ�����ǲ��ᱻ����
            CommandPool(const CommandPool&) = delete;
            CommandPool& operator=(const CommandPool&) = delete;

            // �ṩȫ�ַ��ʵ�
            static Ref<CommandPool> getInstance() {
                std::lock_guard<std::mutex> lock(mutex);
                if (instance == nullptr) {
                    instance = std::make_shared<CommandPoolEnabler>();
                }
                return instance;
            }

            // ��ʼ������
            void init() {
                std::cout << "Initializing command pool..." << std::endl;
                // ��ӳ�ʼ������
            }

            // ��ȡ�����
            void getCommand() {
                std::cout << "Getting command..." << std::endl;
                // ��ӻ�ȡ������߼�
            }
        };


}