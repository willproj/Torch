#pragma once
#include <pch/pch.h>
#include "EditorModule.h"

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
	};
}