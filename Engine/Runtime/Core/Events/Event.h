#pragma once

#include "Runtime/Core/Core.h"

#include "hzpch.h"

namespace Hazel {

	// Events in Hazel are currently blocking, meaning when an event occurs it
	// immediately gets dispatched and must be dealt with right then an there.
	// For the future, a better strategy might be to buffer events in an event
	// bus and process them during the "event" part of the update stage.
	// eventbus ��ûʵ��

	// ������EventType���ֱ��ڲ�ͬ��event����ȥ���崦��
	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved, WindowMaximized, WindowMinimized,
		AppTick, AppUpdate, AppRender, AppActive,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	// eventfilter��
	// BIT() defines in Core.h.
	// ΪʲôҪ��BIT����������Ϊһ��event�����ж�����࣬����button press����mouse����Input
	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};
//		GetStaticType ��Ҫ�ǽ���type�Ƚϵ�ʱ���ÿ��Typeֱ���õģ� ����KeyPressedEvent�����Լ���һ��
// 	   �ྲ̬EventType��
//		GetEventType ���Ǹ�ÿ�������eventȥʹ�õģ�(m_Event.GetEventType() == T::GetStaticType())
#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class HAZEL_API Event
	{
		friend class EventDispatcher;
	public:
		// ������ĳһ��layout�����¼���Ȼ��ֹͣ���Ĵ���
		bool Handled = false;

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		// check wheather in one category
		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
	protected:
		
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event)
			: m_Event(event)
		{
		}
		// T���������һ��EventType�� ����WindowResize
		// ������һ��function��Ӧ�����¼�����Ķ�Ӧ����
		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				// �����function call ���event handler������һ��Cast
				// ִ��������������һش�ִ�н��
				// this �������func�� m_event��placeholder_1
				m_Event.Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}
