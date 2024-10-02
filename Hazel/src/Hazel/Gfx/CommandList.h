#pragma once

namespace Hazel 
{
	class CommandList 
	{
	public:
		virtual ~CommandList() {}
		virtual void OnUpdate() {};
		static Ref<CommandList> Create();

		virtual void Reset() {};
	};

}