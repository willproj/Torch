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

		template<typename... EventTypes>
		bool DispatchAll(const std::function<bool(Event&)>& handler)
		{
			bool handled = false;
			// Iterate over all event types and dispatch
			((handled |= Dispatch<EventTypes>(handler)), ...);
			return handled;
		}
	protected:
		Event& m_Event;
	};

}