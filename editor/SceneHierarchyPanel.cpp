#include "SceneHierarchyPanel.h"
#include <core/renderer/SceneManager.h>
#include <core/renderer/Entity.h>
#include <core/renderer/Component.h>

namespace editor
{
	SceneHierarchyPanel::SceneHierarchyPanel()
	{
	}

	SceneHierarchyPanel::~SceneHierarchyPanel()
	{
	}

    void SceneHierarchyPanel::Render()
    {
        ImGui::Begin("Scene Panel");
        static int selected_fish = -1;
        const char* names[] = { "Create New Entity", "Create Light Entity", "Create", "Create", "Create" };
        static bool toggles[] = { true, false, false, false, false };

        if (ImGui::IsAnyMouseDown() && ImGui::IsWindowHovered())
        {
            if (activeButtonId != -1)
            {
                core::SceneManager::GetSceneManager()->GetSceneRef()->ResetSelectedEntityID();
                activeButtonId = -1;
            }
        }
        

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered())
        {
            ImGui::OpenPopup("my_select_popup");
        }

        ImGui::TextUnformatted(selected_fish == -1 ? "<None>" : names[selected_fish]);

        if (ImGui::BeginPopup("my_select_popup"))
        {
            ImGui::SeparatorText("Creation Panel");
            for (int i = 0; i < IM_ARRAYSIZE(names); i++)
            {
                if (ImGui::Selectable(names[i]))
                {
                    selected_fish = i;
                    ImGui::CloseCurrentPopup(); // Close the popup when an item is selected
                }
            }
            ImGui::EndPopup();
        }

        // Check if the selected item is "Create New Entity"
        if (selected_fish >= 0 && strcmp(names[selected_fish], "Create New Entity") == 0)
        {
            core::SceneManager::GetSceneManager()->CreateEntity();
            selected_fish = -1;
        }

        if (selected_fish >= 0 && strcmp(names[selected_fish], "Create Light Entity") == 0)
        {
            core::SceneManager::GetSceneManager()->CreateEntity(core::EntityType::Light);
            selected_fish = -1;
        }


        float buttonWidth = ImGui::GetContentRegionAvail().x;

        for (auto& entityID : core::SceneManager::GetSceneManager()->GetSceneRef()->GetRegisterRef().view<entt::entity>())
        {
            core::Entity entity = { entityID, &core::SceneManager::GetSceneManager()->GetSceneRef().get() };
            auto& tagComponent = entity.GetComponent<core::LabelComponent>();

            ImGui::PushID((uint32_t)entityID);

            bool isActive = (core::SceneManager::GetSceneManager()->GetSceneRef()->GetSelectedEntityID() == entityID);
            ImVec4 buttonColor = isActive ? ImVec4(1.0f, 0.0f, 0.0f, 1.0f) : ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
            activeButtonId = isActive ? (int)entityID : -1;

            ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.0f, 0.0f, 0.4f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 0.8f));

            ImVec2 textAlign(0.0f, 0.5f); // Align text to the left
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, textAlign);
            if (ImGui::Button(tagComponent.label.c_str(), ImVec2(buttonWidth, 0.0f)))
            {
                core::SceneManager::GetSceneManager()->GetSceneRef()->SetSelectedEntityID(entityID);
                activeButtonId = (int)entityID;
            }
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(3);
            ImGui::PopID();
        }

        ImGui::End();
    }


	void SceneHierarchyPanel::OnUpdate()
	{
	}
}