#include "EntityPropertiesPanel.h"
#include <core/renderer/SceneManager.h>
#include <core/renderer/Entity.h>
#include <core/renderer/Component.h>
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

		core::Entity entity = { selectedEntityId, &core::SceneManager::GetSceneManager()->GetSceneRef().get() };
		if (entity.HasComponent<core::UUIDComponent>())
		{
			auto& uuidComponent = entity.GetComponent<core::UUIDComponent>();
			ImGui::PushTextWrapPos(ImGui::GetContentRegionAvail().x);
			ImGui::Text("UUID: %s", uuidComponent.uuid.ToString().c_str());
			ImGui::PopTextWrapPos();
		}
		
		ImGui::Separator();

		if (entity.HasComponent<core::LabelComponent>())
		{
			auto& labelComponent = entity.GetComponent<core::LabelComponent>();

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

		ImGui::NewLine();

		if (entity.HasComponent<core::ModelComponent>())
		{
			auto& modelComponent = entity.GetComponent<core::ModelComponent>();
			if (ImGui::CollapsingHeader("Model Component"))
			{
				ImGui::Text("Model Name");
			}
		}

		ImGui::End();
	}
	void EntityPropertiesPanel::OnUpdate()
	{
	}
}