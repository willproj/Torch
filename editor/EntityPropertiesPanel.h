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

		void RenderUUIDHeader();
		void RenderLabelHeader();
		void RenderModelHeader();

		void RenderPopupWindow();

		core::Entity m_Entity;

		std::unordered_set<uint32_t> m_AddedComponents;
		std::vector<std::string> m_ComponentsNames = {
			"Add Model Component",
			"Add Color Component"
		};
	};
}