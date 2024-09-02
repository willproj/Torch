#include "Editor.h"
#include <utils/ServiceLocator.h>
#include <utils/Serializer.h>
#include <core/renderer/SceneManager.h>

namespace editor
{
	core::Window *Editor::m_WindowPtr = nullptr;
	std::unordered_map<EditorType, std::unique_ptr<editor::EditorModule>> editor::Editor::m_EditorModules;
	bool Editor::fullscreen = true;
	bool Editor::isOpen = true;

	uint32_t Editor::m_NavbarHeight;
	bool Editor::m_FullScreen = true;

	void Editor::Render()
	{
		DockSpace();

		static bool demo = true;
		ImGui::ShowDemoWindow(&demo);
		for (auto &editor : m_EditorModules)
		{
			editor.second->Render();
		}
		KeyboardShortCut();
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

		ImGuiStyle &style = ImGui::GetStyle();
		style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
		style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		style.Colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
		style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
		style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
		style.GrabRounding = style.FrameRounding = style.TabRounding = 0.0f;
	}

	void Editor::ImGuiBegin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void Editor::ImGuiEnd()
	{
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		ImGuiIO &io = ImGui::GetIO();
		(void)io;
		ImGui::Render();
		// int display_w, display_h;
		// glfwGetFramebufferSize(m_WindowPtr->GetWinSpecification().glfwWindow, &display_w, &display_h);
		// glViewport(0, 0, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow *backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void Editor::OpenSceneFile()
	{
		std::string filepath = utils::FileUtils::OpenFile("Scene File (*.tc)\0*.tc\0");
		if (!filepath.empty())
		{
			utils::Serializer serializer(&core::SceneManager::GetSceneManager()->GetSceneRef().get());
			serializer.DeSerialize(filepath);
		}
	}

	void Editor::SaveSceneFile()
	{
		std::string filePath = utils::FileUtils::SaveFile("Scene File (*.tc)\0*.tc\0");
		if (!filePath.empty())
		{
			utils::Serializer serializer(&core::SceneManager::GetSceneManager()->GetSceneRef().get());
			serializer.Serialize(filePath);
		}
	}

	void Editor::DockSpace()
	{

		// fullscreen = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (m_FullScreen)
		{
			const ImGuiViewport *viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{

			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 10));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("DockSpace Demo", &isOpen, window_flags);

		ImGui::PopStyleVar();
		// if (opt_fullscreen)
		ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO &io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		// render navbar
		RenderNavbar();

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void Editor::RenderNavbar()
	{
		/*render menubar*/
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 10));
		ImGui::BeginMenuBar();

		/* get nav bar height*/
		m_NavbarHeight = ImGui::GetFrameHeight();

		/*render logo*/
		/*ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 8));
		ImGui::ImageButton("logo", (ImTextureID)logo, ImVec2(26, 24));
		ImGui::PopStyleVar();
		ImGui::PopStyleColor(3);*/

		/* double clicked to allow maxmize or restore window */
		/*if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && openglcore::Window::getInstance()->getWindowSpecification().isDoubleClicked)
		{
			resetWindowSizePos(switchIcon);
		}*/

		/* set menu items */
		if (ImGui::BeginMenu("Files"))
		{
			if (ImGui::MenuItem("Open File			Ctrl+O"))
			{
				OpenSceneFile();
			}

			if (ImGui::MenuItem("Save File			Ctrl+S"))
			{
				SaveSceneFile();
			}

			if (ImGui::MenuItem("EXIT				Alt+F4"))
			{
				// std::string filePath = utils::FileSystem::openFolder();
			}
			ImGui::MenuItem("EXIT				Alt+F4");
			ImGui::EndMenu();
		}

		/*if (ImGui::BeginMenu("View"))
		{
			for (auto& gui : guiStack)
			{
				ImGui::MenuItem(gui->getUiName().c_str(), nullptr, gui->getIsOpenPtr());
			}

			ImGui::EndMenu();
		}*/

		if (ImGui::BeginMenu("Settings"))
		{
			ImGui::MenuItem("Docking Space", nullptr, &m_FullScreen);
			auto context = utils::ServiceLocator::GetGraphicsContext();
			auto currentRenderType = context->GetRenderType();

			if (ImGui::MenuItem("Render All", nullptr, currentRenderType == core::GBufferRenderType::All))
			{
				context->SetUpRenderType(core::GBufferRenderType::All);
			}

			if (ImGui::MenuItem("Render GBuffer Color", nullptr, currentRenderType == core::GBufferRenderType::GColor))
			{
				context->SetUpRenderType(core::GBufferRenderType::GColor);
			}

			if (ImGui::MenuItem("Render GBuffer Position", nullptr, currentRenderType == core::GBufferRenderType::GPosition))
			{
				context->SetUpRenderType(core::GBufferRenderType::GPosition);
			}

			if (ImGui::MenuItem("Render GBuffer Normal", nullptr, currentRenderType == core::GBufferRenderType::GNormal))
			{
				context->SetUpRenderType(core::GBufferRenderType::GNormal);
			}

			if (ImGui::MenuItem("Render GBuffer Depth", nullptr, currentRenderType == core::GBufferRenderType::GDepth))
			{
				context->SetUpRenderType(core::GBufferRenderType::GDepth);
			}

			ImGui::EndMenu();
		}


		/* do offset for window control icons */
		auto navBarWidth = ImGui::GetWindowSize().x;
		auto offset = navBarWidth - 350;
		ImGui::Dummy(ImVec2(offset, 0));

		/* render window control icons */
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 8));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		// if (ImGui::ImageButton("minimize", (ImTextureID)minimizeIcon, ImVec2(26, 26)))
		//{
		//	glfwIconifyWindow(openglcore::Window::getInstance()->getGLFWwinPtr());
		// }

		// auto tex = switchIcon == 1 ? maximizeIcon : restoreIcon;
		// if (ImGui::ImageButton("maximize or restore", (ImTextureID)tex, ImVec2(26, 26)))
		//{
		//	resetWindowSizePos(switchIcon);
		// }

		// if (ImGui::ImageButton("close", (ImTextureID)closeIcon, ImVec2(26, 26)))
		//{
		//	//TO DO
		// }

		ImGui::PopStyleColor(2);
		ImGui::PopStyleVar(2);

		ImGui::EndMenuBar();
		ImGui::PopStyleVar();
	}

	void Editor::KeyboardShortCut()
	{
		auto keyboard = utils::ServiceLocator::GetKeyboard();
		if (keyboard->IsKeyPressed(core::KeyCode::KEY_LEFT_CONTROL) && keyboard->IsKeyPressed(core::KeyCode::KEY_O))
		{
			OpenSceneFile();
		}

		if (keyboard->IsKeyPressed(core::KeyCode::KEY_LEFT_CONTROL) && keyboard->IsKeyPressed(core::KeyCode::KEY_S))
		{
			SaveSceneFile();
		}
	}

	void Editor::AddModule(EditorType type, std::unique_ptr<EditorModule> module)
	{
		m_EditorModules[type] = std::move(module);
	}
}
