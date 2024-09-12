#include "EntityPropertiesPanel.h"
#include <core/renderer/SceneManager.h>
#include <core/renderer/Entity.h>
#include <core/renderer/Component.h>
#include <utils/FileUtils.h>
#include <core/renderer/ModelManager.h>
#include <imgui_internal.h>
#include <core/glcore/Texture.h>

namespace editor
{
	EntityPropertiesPanel::EntityPropertiesPanel()
	{
		m_DefaultTexture = core::Texture::LoadTexture(std::string(PROJECT_ROOT) + "/assets/texture/default.jpg");
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
			m_Entity = core::Entity();
			ImGui::End();
			return;
		}

		m_Entity = { selectedEntityId, &core::SceneManager::GetSceneManager()->GetSceneRef().get() };

		RenderUUIDHeader();
		RenderEntityType();
		ImGui::Separator();

		RenderLabelHeader();
		RenderModelHeader();
		RenderTransformHeader();
		RenderMaterialHeader();

		RenderPopupWindow();

		ImGui::End();
	}
	void EntityPropertiesPanel::OnUpdate()
	{
	}

	void EntityPropertiesPanel::RenderEntityType()
	{
		if (m_Entity.HasComponent<core::EntityTypeComponent>())
		{
			auto& typeComponent = m_Entity.GetComponent<core::EntityTypeComponent>();
			ImGui::Text("Entity Type: %s", typeComponent.entityType == core::EntityType::General ? "General" : "Light");
		}
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
		auto& modelComponent = m_Entity.GetComponent<core::ModelComponent>();
		ImGui::PushTextWrapPos(ImGui::GetContentRegionAvail().x);
		ImGui::Text("Model: "); ImGui::SameLine();
		if (modelComponent.model != nullptr)
		{
			ImGui::Text(modelComponent.model->GetModelPath().c_str());
		}
		ImGui::PopTextWrapPos();
		
		if (ImGui::Button("Load Model"))
		{
			m_ModelFilePath = utils::FileUtils::OpenFile("GLTF/GLB Files (*.gltf;*.glb)\0*.gltf;*.glb\0All Files (*.*)\0*.*\0");
			core::ModelManager::GetInstance()->LoadModel(m_ModelFilePath);
			if (m_Entity.HasComponent<core::ModelComponent>())
			{
				auto& modelComponent = m_Entity.GetComponent<core::ModelComponent>();
				modelComponent.model = core::ModelManager::GetInstance()->GetModel(m_ModelFilePath);
			}
		} 
		ImGui::NewLine();
	}
	void EntityPropertiesPanel::RenderTransformHeader()
	{
		if (m_Entity.HasComponent<core::TransformComponent>())
		{
			auto& transform = m_Entity.GetComponent<core::TransformComponent>();
			if (ImGui::CollapsingHeader("Transform Component", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
				DrawVec3Control("Translation", transform.translation);
				ImGui::Spacing();
				glm::vec3 rotation = glm::degrees(transform.rotation);
				DrawVec3Control("Rotation", rotation);
				ImGui::Spacing();
				transform.rotation = glm::radians(rotation);
				DrawVec3Control("Scale", transform.scale, 1.0f);
				ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
			}
		}
	}
	void EntityPropertiesPanel::RenderMaterialHeader()
	{
		if (m_Entity.HasComponent<core::MaterialComponent>())
		{
			auto& material = m_Entity.GetComponent<core::MaterialComponent>();
			if (ImGui::CollapsingHeader("Material Component", ImGuiTreeNodeFlags_DefaultOpen))
			{
				uint32_t imageSize = 100;
				ImGui::Checkbox("Use Albedo", &material.useAlbedoTexture);
				ImGui::PushID(1);
				if (ImGui::ImageButton((void*)(intptr_t)(material.albedoTexture == 0 ? m_DefaultTexture : material.albedoTexture), ImVec2(imageSize, imageSize)))
				{
					const std::string path = utils::FileUtils::OpenFile("Image Files (*.jpg;*.png)\0*.jpg;*.png\0");
					if (!path.empty())
					{
						material.albedoPath = path;
						material.albedoTexture = core::Texture::LoadTexture(path);
					}
				}
				ImGui::PopID();
				ImGui::Separator();

				ImGui::Checkbox("Use Normal", &material.useNormalTexture);
				ImGui::PushID(2);
				if (ImGui::ImageButton((void*)(intptr_t)(material.normalTexture == 0 ? m_DefaultTexture : material.normalTexture), ImVec2(imageSize, imageSize)))
				{
					const std::string path = utils::FileUtils::OpenFile("Image Files (*.jpg;*.png)\0*.jpg;*.png\0");
					if (!path.empty())
					{
						material.normalPath = path;
						material.normalTexture = core::Texture::LoadTexture(path);
					}
				}
				ImGui::PopID();
				ImGui::Separator();

				ImGui::Checkbox("Use Metallic", &material.useMetallicTexture);
				ImGui::PushID(3);
				if (ImGui::ImageButton((void*)(intptr_t)(material.metallicTexture == 0 ? m_DefaultTexture : material.metallicTexture), ImVec2(imageSize, imageSize)))
				{
					const std::string path = utils::FileUtils::OpenFile("Image Files (*.jpg;*.png)\0*.jpg;*.png\0");
					if (!path.empty())
					{
						material.metallicPath = path;
						material.metallicTexture = core::Texture::LoadTexture(path);
					}
				}
				ImGui::SliderFloat("Metallic", &material.metallic, 0.0f, 1.0f);
				ImGui::PopID();
				ImGui::Separator();

				ImGui::Checkbox("Use Roughness", &material.useRoughnessTexture);
				ImGui::PushID(4);
				if (ImGui::ImageButton((void*)(intptr_t)(material.roughnessTexture == 0 ? m_DefaultTexture : material.roughnessTexture), ImVec2(imageSize, imageSize)))
				{
					const std::string path = utils::FileUtils::OpenFile("Image Files (*.jpg;*.png)\0*.jpg;*.png\0");
					if (!path.empty())
					{
						material.roughnessPath = path;
						material.roughnessTexture = core::Texture::LoadTexture(path);
					}
				}
				ImGui::SliderFloat("Roughness", &material.roughness, 0.0f, 10.0f);
				ImGui::PopID();
				ImGui::Separator();

				ImGui::Checkbox("Use AO", &material.useAOTexture);
				ImGui::PushID(5);
				if (ImGui::ImageButton((void*)(intptr_t)(material.aoTexture == 0 ? m_DefaultTexture : material.aoTexture), ImVec2(imageSize, imageSize)))
				{
					const std::string path = utils::FileUtils::OpenFile("Image Files (*.jpg;*.png)\0*.jpg;*.png\0");
					if (!path.empty())
					{
						material.aoPath = path;
						material.aoTexture = core::Texture::LoadTexture(path);
					}
				}
				ImGui::PopID();
				ImGui::Separator();
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
			bool hasComponent = m_Entity.HasComponent<core::MaterialComponent>();
			if (hasComponent)
			{
				ImGui::BeginDisabled();
			}
			if (ImGui::Selectable("Material Component", !hasComponent))
			{
				m_Entity.AddComponent<core::MaterialComponent>();
				ImGui::CloseCurrentPopup();
			}
			if (hasComponent)
			{
				ImGui::EndDisabled();
			}

			ImGui::EndPopup();
		}
	}
	void EntityPropertiesPanel::DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });



		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}
}