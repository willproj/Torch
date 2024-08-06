#include "TorchGraphicsContext.h"
#include "TorchOpenGLContext.h"
#include "TorchVulkanContext.h"

namespace core
{
    std::unique_ptr<TorchGraphicsContext> TorchGraphicsContext::GetGraphicsContext()
    {
#if defined(TORCH_ENGINE_API_VULKAN)
        return std::make_unique<TorchVulkanContext>();
#elif defined(TORCH_ENGINE_API_OPENGL)
        return std::make_unique<TorchOpenGLContext>();
#else
        return nullptr; 
#endif
    }
}
