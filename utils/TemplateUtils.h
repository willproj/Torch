#pragma once

namespace core
{
	template<typename T>
	inline T& singleton()
	{
		static T instance;
		return instance;
	}
}