#pragma once
#include <pch/pch.h>
#include "EditorModule.h"
#include <core/renderer/Entity.h>

namespace editor
{
	class EntityPropertiesPanel : public EditorModule
	{
	public:
		EntityPropertiesPanel();
		~EntityPropertiesPanel();

		void Render() override;
		void OnUpdate() override;
	private:
		void RenderEntityType();
		void RenderUUIDHeader();
		void RenderLabelHeader();
		void RenderModelHeader();
		void RenderTransformHeader();
		void RenderMaterialHeader();

		void RenderPopupWindow();

		static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);

		core::Entity m_Entity;
		std::string m_ModelFilePath = "";
		uint32_t m_DefaultTexture;
	};
}