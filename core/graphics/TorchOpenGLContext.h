#pragma once
#include <pch/pch.h>
#include "TorchGraphicsContext.h"

namespace core
{
	class TorchOpenGLContext :public TorchGraphicsContext
	{
	public:
		TorchOpenGLContext();
		~TorchOpenGLContext();

		void DrawFrame() override;
	private:

	};
}