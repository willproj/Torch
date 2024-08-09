#pragma once
#include "Event.h"	


namespace core
{
	class EventDispatcher
	{
	public:
		EventDispatcher(Event& event) : m_Event(event) {}

		//dispatcher event type with a callable handle
		template<typename EventType>
		bool Dispatch(const std::function<bool(EventType&)>& handler)
		{
			if (m_Event.GetType() == EventType::GetStaticType())
			{
				m_Event.SetHandled(handler(static_cast<EventType&>(m_Event)));
				return true;
			}
			return false;
		}
	protected:
		Event& m_Event;
	};

}