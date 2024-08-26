#pragma once
#include <pch/pch.h>
#include "EditorModule.h"
namespace editor
{
	class SceneHierarchyPanel : public EditorModule
	{
	public:
		SceneHierarchyPanel();
		~SceneHierarchyPanel();

		void Render() override;
		void OnUpdate() override;
		
	private:
		int activeButtonId = -1;
	};
}