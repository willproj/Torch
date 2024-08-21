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

		void DrawFrame();
		APIType GetAPIType() const override { return APIType::OpenGL; }
	private:

	};
}