#pragma once
#include "EditorModule.h"
#include <core/renderer/EnvironmentManager.h>

namespace editor
{
	class EnvironmentPropertiesPanel : public EditorModule
	{
	public:
		EnvironmentPropertiesPanel() = default;
		~EnvironmentPropertiesPanel() = default;

		void Render() override;
		void OnUpdate() override;

	private:
		void RenderAtmosphereHeader();
		std::shared_ptr<core::EnvironmentManager> m_EnvirManager;
	};
}