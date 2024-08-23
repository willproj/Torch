#include "Editor.h"
#include <utils/ServiceLocator.h>

namespace editor
{
	core::Window *Editor::m_WindowPtr = nullptr;

	void Editor::Render()
	{
		for (auto &editor : m_EditorModules)
		{
			editor->Render();
		}
	}

	void Editor::SetUpImGui()
	{
		// Setup Dear ImGui context
		m_WindowPtr = utils::ServiceLocator::GetWindow();
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;	  // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;	  // Enable Multi-Viewport / Platform Windows

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		// ImGui::StyleColorsLight();

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(m_WindowPtr->GetWinSpecification().glfwWindow, true);
#ifdef __EMSCRIPTEN__
		ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
		ImGui_ImplOpenGL3_Init("#version 450");
	}

	void Editor::ImGuiBegin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		static bool open = true;
		ImGui::ShowDemoWindow(&open);
	}

	void Editor::ImGuiEnd()
	{
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		ImGuiIO &io = ImGui::GetIO();
		(void)io;
		ImGui::Render();
		//int display_w, display_h;
		//glfwGetFramebufferSize(m_WindowPtr->GetWinSpecification().glfwWindow, &display_w, &display_h);
		//glViewport(0, 0, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow *backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void Editor::AddModule(std::unique_ptr<EditorModule> module)
	{
		m_EditorModules.emplace_back(std::move(module));
	}
}
