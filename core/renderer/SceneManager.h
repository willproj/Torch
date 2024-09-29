#pragma once
#include <pch/pch.h>
#include "Scene.h"
#include "camera/EditorCamera.h"
#include "core/glcore/Shader.h"
#include "ModelManager.h"

namespace core
{
	class Shader;
	class SceneManager
	{
	public:
		static std::shared_ptr<SceneManager> GetSceneManager();
		~SceneManager();

		void CreateEntity(EntityType type = EntityType::General);
		void RemoveEntity(Entity entity);

		void Render();

		utils::Ref<Scene> GetSceneRef() { return utils::Ref<Scene>(m_Scene); }
		void SetCamera(std::shared_ptr<EditorCamera> camera);
		std::shared_ptr<EditorCamera> GetEditorCameraPtr() const { return m_EditorCameraPtr; }

		void SetScene(Scene&& scene);

		void RenderScene();

	private:
		SceneManager();
		Scene m_Scene;
		static std::shared_ptr<SceneManager> m_SceneManager;
		std::shared_ptr<EditorCamera> m_EditorCameraPtr;
	};
}