#pragma once
#include <pch/pch.h>
#include "Scene.h"

namespace core
{
	class Shader;
	class SceneManager
	{
	public:
		static std::shared_ptr<SceneManager> GetSceneManager();
		~SceneManager();

		void CreateEntity();
		void RemoveEntity(Entity entity);

		void Render(Shader& shader);

		utils::Ref<Scene> GetSceneRef() { return utils::Ref<Scene>(m_Scene); }

	private:
		SceneManager();
		Scene m_Scene;
		static std::shared_ptr<SceneManager> m_SceneManager;
	};
}