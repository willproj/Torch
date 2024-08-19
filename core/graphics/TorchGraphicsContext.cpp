#include "TorchGraphicsContext.h"
#include "TorchOpenGLContext.h"
#include "TorchVulkanContext.h"

namespace core
{
	std::unique_ptr<TorchGraphicsContext> TorchGraphicsContext::s_GraphicsContextInstance = nullptr;

	std::unique_ptr<TorchGraphicsContext>& TorchGraphicsContext::GetGraphicsContext()
	{
		if (!s_GraphicsContextInstance)
		{
#if defined(TORCH_ENGINE_API_VULKAN) 
			s_GraphicsContextInstance = std::make_unique<TorchVulkanContext>();
#elif defined(TORCH_ENGINE_API_OPENGL)
			s_GraphicsContextInstance = std::make_unique<TorchOpenGLContext>();
#else
			return nullptr;
#endif
		}
		return s_GraphicsContextInstance;
	}
}
