#include "EntityPropertiesPanel.h"
#include <core/renderer/SceneManager.h>
#include <core/renderer/Entity.h>
#include <core/renderer/Component.h>
#include <utils/FileUtils.h>
#include <core/renderer/ModelManager.h>

namespace editor
{
	EntityPropertiesPanel::EntityPropertiesPanel()
	{
	}
	EntityPropertiesPanel::~EntityPropertiesPanel()
	{
	}
	void EntityPropertiesPanel::Render()
	{
		ImGui::Begin("Entity Properties");

		auto selectedEntityId = core::SceneManager::GetSceneManager()->GetSceneRef()->GetSelectedEntityID();
		if (selectedEntityId == entt::null)
		{
			ImGui::End();
			return;
		}

		m_Entity = { selectedEntityId, &core::SceneManager::GetSceneManager()->GetSceneRef().get() };

		RenderUUIDHeader();
		ImGui::Separator();

		RenderLabelHeader();
		ImGui::NewLine();

		RenderModelHeader();

		RenderPopupWindow();

		ImGui::End();
	}
	void EntityPropertiesPanel::OnUpdate()
	{
	}

	void EntityPropertiesPanel::RenderUUIDHeader()
	{
		if (m_Entity.HasComponent<core::UUIDComponent>())
		{
			auto& uuidComponent = m_Entity.GetComponent<core::UUIDComponent>();
			ImGui::PushTextWrapPos(ImGui::GetContentRegionAvail().x);
			ImGui::Text("UUID: %s", uuidComponent.uuid.ToString().c_str());
			ImGui::PopTextWrapPos();
		}
	}

	void EntityPropertiesPanel::RenderLabelHeader()
	{
		if (m_Entity.HasComponent<core::LabelComponent>())
		{
			auto& labelComponent = m_Entity.GetComponent<core::LabelComponent>();

			static char str0[256];
			strncpy(str0, labelComponent.label.c_str(), sizeof(str0) - 1);
			str0[sizeof(str0) - 1] = '\0';

			ImGui::PushItemWidth(200);
			ImGui::Text("Label: ");
			ImGui::PushID(1);
			if (ImGui::InputText("", str0, sizeof(str0)))
			{
				labelComponent.label = str0;
			}
			ImGui::PopID();
			ImGui::PopItemWidth();
		}
	}
	void EntityPropertiesPanel::RenderModelHeader()
	{
		if (m_Entity.HasComponent<core::ModelComponent>())
		{
			auto& modelComponent = m_Entity.GetComponent<core::ModelComponent>();
			if (ImGui::CollapsingHeader("Model Component"))
			{
				if (ImGui::Button("Load Model"))
				{
					const std::string filePath = utils::FileUtils::OpenFile("gltf File (*.gltf)\0*.gltf\0");
					core::ModelManager::GetInstance()->LoadModel(filePath);
					if (m_Entity.HasComponent<core::ModelComponent>())
					{
						auto& modelComponent = m_Entity.GetComponent<core::ModelComponent>();
						modelComponent.model = core::ModelManager::GetInstance()->GetModel(filePath);
					}
				} 
			}
		}
	}
	void EntityPropertiesPanel::RenderPopupWindow()
	{
		// Track added components using a set
		uint32_t selected = -1;

		if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup("Create Component Window");
		}

		if (ImGui::BeginPopup("Create Component Window"))
		{
			ImGui::SeparatorText("Components");
			for (int i = 0; i < m_ComponentsNames.size(); i++)
			{
				bool alreadyAdded = m_AddedComponents.find(i) != m_AddedComponents.end();

				// Disable the selectable if the component has already been added
				if (alreadyAdded)
				{
					ImGui::BeginDisabled();
				}

				auto componentName = m_ComponentsNames[i];
				if (ImGui::Selectable(componentName.c_str(), !alreadyAdded))
				{
					// Only allow selection if it hasn't already been added
					if (!alreadyAdded)
					{
						if (componentName == "Add Model Component")
						{
							m_Entity.AddComponent<core::ModelComponent>();
						}
						else if (componentName == "Add Color Component")
						{

						}

						selected = i;
						m_AddedComponents.insert(i);  // Mark the component as added
						ImGui::CloseCurrentPopup(); // Close the popup when an item is selected
					}
				}

				if (alreadyAdded)
				{
					ImGui::EndDisabled();
				}
			}

			ImGui::EndPopup();
		}
	}

	

}