#include "Editor.h"
#include <utils/ServiceLocator.h>

namespace editor
{
	ImGui_ImplVulkanH_Window Editor::m_MainWindowData;
	int Editor::m_MinImageCount = 2;
	bool Editor::m_SwapChainRebuild = false;
	core::TorchVulkanContext* Editor::m_GraphicsContextPtr = nullptr;

	void Editor::FrameRender(ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data)
	{
		VkResult err;
		m_GraphicsContextPtr = dynamic_cast<core::TorchVulkanContext*>(utils::ServiceLocator::GetGraphicsContext());
		VkSemaphore image_acquired_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
		VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
		err = vkAcquireNextImageKHR(m_GraphicsContextPtr->GetLogicalDevice().GetLogicDevice(), wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd->FrameIndex);
		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
		{
			m_SwapChainRebuild = true;
			return;
		}

		ImGui_ImplVulkanH_Frame* fd = &wd->Frames[wd->FrameIndex];
		{
			err = vkWaitForFences(m_GraphicsContextPtr->GetLogicalDevice().GetLogicDevice(), 1, &fd->Fence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking

			err = vkResetFences(m_GraphicsContextPtr->GetLogicalDevice().GetLogicDevice(), 1, &fd->Fence);
		}
		{
			err = vkResetCommandPool(m_GraphicsContextPtr->GetLogicalDevice().GetLogicDevice(), fd->CommandPool, 0);
			VkCommandBufferBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
		}
		{
			VkRenderPassBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			info.renderPass = wd->RenderPass;
			info.framebuffer = fd->Framebuffer;
			info.renderArea.extent.width = wd->Width;
			info.renderArea.extent.height = wd->Height;
			info.clearValueCount = 1;
			info.pClearValues = &wd->ClearValue;
			vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
		}

		// Record dear imgui primitives into command buffer
		ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

		// Submit command buffer
		vkCmdEndRenderPass(fd->CommandBuffer);
		{
			VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			VkSubmitInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			info.waitSemaphoreCount = 1;
			info.pWaitSemaphores = &image_acquired_semaphore;
			info.pWaitDstStageMask = &wait_stage;
			info.commandBufferCount = 1;
			info.pCommandBuffers = &fd->CommandBuffer;
			info.signalSemaphoreCount = 1;
			info.pSignalSemaphores = &render_complete_semaphore;

			err = vkEndCommandBuffer(fd->CommandBuffer);
			err = vkQueueSubmit(m_GraphicsContextPtr->GetLogicalDevice().GetGraphicsQueue(), 1, &info, fd->Fence);
		}
	}

	void Editor::FramePresent(ImGui_ImplVulkanH_Window* wd)
	{
		if (m_SwapChainRebuild)
			return;
		VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
		VkPresentInfoKHR info = {};
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = &render_complete_semaphore;
		info.swapchainCount = 1;
		info.pSwapchains = &wd->Swapchain;
		info.pImageIndices = &wd->FrameIndex;
		VkResult err = vkQueuePresentKHR(m_GraphicsContextPtr->GetLogicalDevice().GetGraphicsQueue(), &info);
		if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
		{
			m_SwapChainRebuild = true;
			return;
		}
		wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->SemaphoreCount; // Now we can use the next set of semaphores
	}


	void Editor::Render()
	{
		for (auto& editor : m_EditorModules)
		{
			editor->Render();
		}
	}

	void Editor::SetUpImGui()
	{
		int w, h;
		m_GraphicsContextPtr = dynamic_cast<core::TorchVulkanContext*>(utils::ServiceLocator::GetGraphicsContext());
		auto appWindow = utils::ServiceLocator::GetWindow();
		glfwGetFramebufferSize(appWindow->GetWinSpecification().glfwWindow, &w, &h);
		ImGui_ImplVulkanH_Window* wd = &m_MainWindowData;
		wd->Surface = m_GraphicsContextPtr->GetSurface().GetVulkanSurface();
		wd->Swapchain = m_GraphicsContextPtr->GetSwapChain().GetSwapChain();
		auto queueFamily = core::VulkanCommon::FindQueueFamilies(m_GraphicsContextPtr->GetPhysicalDevice().GetVulkanPhysicalDevice(), m_GraphicsContextPtr->GetSurface().GetVulkanSurface()).graphicsFamily.value();

		// Check for WSI support
		VkBool32 res;
		vkGetPhysicalDeviceSurfaceSupportKHR(m_GraphicsContextPtr->GetPhysicalDevice().GetVulkanPhysicalDevice(), queueFamily, m_GraphicsContextPtr->GetSurface().GetVulkanSurface(), &res);
		if (res != VK_TRUE)
		{
			fprintf(stderr, "Error no WSI support on physical device 0\n");
			exit(-1);
		}

		//Select Surface Format
		//const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
		const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_SRGB };
		const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(m_GraphicsContextPtr->GetPhysicalDevice().GetVulkanPhysicalDevice(), m_GraphicsContextPtr->GetSurface().GetVulkanSurface(), requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

		// Select Present Mode
#ifdef APP_USE_UNLIMITED_FRAME_RATE
		VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
		//VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
		VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR };
#endif
		wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(m_GraphicsContextPtr->GetPhysicalDevice().GetVulkanPhysicalDevice(), m_GraphicsContextPtr->GetSurface().GetVulkanSurface(), &present_modes[0], IM_ARRAYSIZE(present_modes));
		//printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

		auto instance = m_GraphicsContextPtr->GetInstance();
		// Create SwapChain, RenderPass, Framebuffer, etc.
		ImGui_ImplVulkanH_CreateOrResizeWindow
		(
			m_GraphicsContextPtr->GetInstance().GetVulkanInstance(),
			m_GraphicsContextPtr->GetPhysicalDevice().GetVulkanPhysicalDevice(),
			m_GraphicsContextPtr->GetLogicalDevice().GetLogicDevice(),
			wd,
			queueFamily,
			instance.GetAllocator(), 
			w,
			h,
			m_MinImageCount
		);
//
//		// Setup Dear ImGui context
//		IMGUI_CHECKVERSION();
//		ImGui::CreateContext();
//		ImGuiIO& io = ImGui::GetIO(); (void)io;
//		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
//		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
//		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
//		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
//		//io.ConfigViewportsNoAutoMerge = true;
//		//io.ConfigViewportsNoTaskBarIcon = true;
//
//		// Setup Dear ImGui style
//		ImGui::StyleColorsDark();
//		//ImGui::StyleColorsLight();
//
//		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
//		ImGuiStyle& style = ImGui::GetStyle();
//		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
//		{
//			style.WindowRounding = 0.0f;
//			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
//		}
//
//		// Setup Platform/Renderer backends
//		static VkPipelineCache g_PipelineCache = VK_NULL_HANDLE;
//		static VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;
//		{
//			VkDescriptorPoolSize pool_sizes[] =
//			{
//				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
//			};
//			VkDescriptorPoolCreateInfo pool_info = {};
//			pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//			pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
//			pool_info.maxSets = 1;
//			pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
//			pool_info.pPoolSizes = pool_sizes;
//			vkCreateDescriptorPool(m_GraphicsContextPtr->GetLogicalDevice().GetLogicDevice(), &pool_info, instance.GetAllocator(), &g_DescriptorPool);
//		}
//		ImGui_ImplGlfw_InitForVulkan(appWindow->GetWinSpecification().glfwWindow, true);
//		ImGui_ImplVulkan_InitInfo init_info = {};
//		init_info.Instance = m_GraphicsContextPtr->GetInstance().GetVulkanInstance();
//		init_info.PhysicalDevice = m_GraphicsContextPtr->GetPhysicalDevice().GetVulkanPhysicalDevice();
//		init_info.Device = m_GraphicsContextPtr->GetLogicalDevice().GetLogicDevice();
//		init_info.QueueFamily = core::VulkanCommon::FindQueueFamilies(m_GraphicsContextPtr->GetPhysicalDevice().GetVulkanPhysicalDevice(), m_GraphicsContextPtr->GetSurface().GetVulkanSurface()).graphicsFamily.value();
//		init_info.Queue = m_GraphicsContextPtr->GetLogicalDevice().GetGraphicsQueue();
//		init_info.PipelineCache = g_PipelineCache;
//		init_info.DescriptorPool = g_DescriptorPool;
//		init_info.RenderPass = m_GraphicsContextPtr->GetRenderPass().GetVulkanRenderPass();
//		init_info.Subpass = 0;
//		init_info.MinImageCount = m_MinImageCount;
//		init_info.ImageCount = wd->ImageCount;
//		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
//		init_info.Allocator = instance.GetAllocator();
//		//init_info.CheckVkResultFn = check_vk_result;
//		ImGui_ImplVulkan_Init(&init_info);
//
//
//		// Load Fonts
//		// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
//		// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
//		// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
//		// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
//		// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
//		// - Read 'docs/FONTS.md' for more instructions and details.
//		// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
//		//io.Fonts->AddFontDefault();
//		//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
//		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
//		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
//		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
//		//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
//		//IM_ASSERT(font != nullptr);
//
//		// Our state
		

	}

	void Editor::ImGuiBegin()
	{
		// Resize swap chain?
		int fb_width, fb_height;
		auto winSpec = utils::ServiceLocator::GetWindow()->GetWinSpecification();
		auto instance = m_GraphicsContextPtr->GetInstance();
		glfwGetFramebufferSize(winSpec.glfwWindow, &fb_width, &fb_height);
		if (fb_width > 0 && fb_height > 0 && (m_SwapChainRebuild || m_MainWindowData.Width != fb_width || m_MainWindowData.Height != fb_height))
		{
			auto queueFamily = core::VulkanCommon::FindQueueFamilies(m_GraphicsContextPtr->GetPhysicalDevice().GetVulkanPhysicalDevice(), m_GraphicsContextPtr->GetSurface().GetVulkanSurface()).graphicsFamily.value();
			ImGui_ImplVulkan_SetMinImageCount(m_MinImageCount);
			ImGui_ImplVulkanH_CreateOrResizeWindow
			(
				m_GraphicsContextPtr->GetInstance().GetVulkanInstance(),
				m_GraphicsContextPtr->GetPhysicalDevice().GetVulkanPhysicalDevice(),
				m_GraphicsContextPtr->GetLogicalDevice().GetLogicDevice(),
				&m_MainWindowData,
				queueFamily,
				instance.GetAllocator(),
				fb_width,
				fb_height,
				m_MinImageCount
			);
			m_MainWindowData.FrameIndex = 0;
			m_SwapChainRebuild = false;
		}
		if (glfwGetWindowAttrib(winSpec.glfwWindow, GLFW_ICONIFIED) != 0)
		{
			ImGui_ImplGlfw_Sleep(10);
		}

		// Start the Dear ImGui frame
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		ImGui_ImplVulkanH_Window* wd = &m_MainWindowData;
		ImDrawData* main_draw_data = ImGui::GetDrawData();
		const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);
		wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
		wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
		wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
		wd->ClearValue.color.float32[3] = clear_color.w;
		if (!main_is_minimized)
			FrameRender(wd, main_draw_data);

		// Update and Render additional Platform Windows
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		// Present Main Platform Window
		if (!main_is_minimized)
			FramePresent(wd);
	}

	void Editor::ImGuiEnd()
	{
		// Cleanup
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void Editor::AddModule(std::unique_ptr<EditorModule> module)
	{
		m_EditorModules.emplace_back(std::move(module));
	}
}

