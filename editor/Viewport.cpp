#include "Viewport.h"
#include <utils/ServiceLocator.h>
#include <core/renderer/SceneManager.h>
#include <ImGuizmo.h>
#include <core/renderer/Component.h>

namespace editor
{
	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
	{
		// From glm::decompose in matrix_decompose.inl

		using namespace glm;
		using T = float;

		mat4 LocalMatrix(transform);

		// Normalize the matrix.
		if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
			return false;

		// First, isolate perspective.  This is the messiest.
		if (
			epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
		{
			// Clear the perspective partition
			LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
			LocalMatrix[3][3] = static_cast<T>(1);
		}

		// Next take care of translation (easy).
		translation = vec3(LocalMatrix[3]);
		LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

		vec3 Row[3], Pdum3;

		// Now get scale and shear.
		for (length_t i = 0; i < 3; ++i)
			for (length_t j = 0; j < 3; ++j)
				Row[i][j] = LocalMatrix[i][j];

		// Compute X scale factor and normalize first row.
		scale.x = length(Row[0]);
		Row[0] = detail::scale(Row[0], static_cast<T>(1));
		scale.y = length(Row[1]);
		Row[1] = detail::scale(Row[1], static_cast<T>(1));
		scale.z = length(Row[2]);
		Row[2] = detail::scale(Row[2], static_cast<T>(1));

		// At this point, the matrix (in rows[]) is orthonormal.
		// Check for a coordinate system flip.  If the determinant
		// is -1, then negate the matrix and the scaling factors.
#if 0
		Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
		if (dot(Row[0], Pdum3) < 0)
		{
			for (length_t i = 0; i < 3; i++)
			{
				scale[i] *= static_cast<T>(-1);
				Row[i] *= static_cast<T>(-1);
			}
		}
#endif

		rotation.y = asin(-Row[0][2]);
		if (cos(rotation.y) != 0) {
			rotation.x = atan2(Row[1][2], Row[2][2]);
			rotation.z = atan2(Row[0][1], Row[0][0]);
		}
		else {
			rotation.x = atan2(-Row[2][0], Row[1][1]);
			rotation.z = 0;
		}


		return true;
	}

	void Viewport::Render()
	{
		auto context = utils::ServiceLocator::GetGraphicsContext();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f)); 
		ImGui::Begin("viewport");

		ImVec2 viewportPanel = ImGui::GetContentRegionAvail();
		if (m_ViewportSize != *((glm::vec2*)&viewportPanel))
		{
			m_ViewportSize = glm::vec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
			context->OnUpdate(m_ViewportSize.x, m_ViewportSize.y);
			glViewport(0, 0, m_ViewportSize.x, m_ViewportSize.y);
		}

		// Display RedInt texture
		ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(context->GetScreenTexture())),
			ImVec2(m_ViewportSize.x, m_ViewportSize.y),  
			ImVec2(0, 1),       // UV top-left
			ImVec2(1, 0));      // UV bottom-right


		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		viewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		viewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
		
		
		auto [x, y] = ImGui::GetMousePos();
		x -= viewportBounds[0].x;
		y -= viewportBounds[0].y;
		y = m_ViewportSize.y - y;

		auto keyboard = utils::ServiceLocator::GetKeyboard();
		

		auto sceneRef = core::SceneManager::GetSceneManager()->GetSceneRef();
		auto selectedEntity = sceneRef->GetSelectedEntityID();

		viewportMinRegion = ImGui::GetWindowContentRegionMin();
		viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		viewportOffset = ImGui::GetWindowPos();
		viewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		viewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
		//
		int32_t guizmoType = -1;
		core::Entity entity = { selectedEntity, &sceneRef.get()};
		auto mouse = utils::ServiceLocator::GetMouse();
		if (!entity.IsEntityEmpty()
			&& entity.HasComponent<core::TransformComponent>()
			&& !keyboard->IsKeyPressed(core::KeyCode::KEY_LEFT_CONTROL)
			)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(viewportBounds[0].x, viewportBounds[0].y, viewportBounds[1].x - viewportBounds[0].x, viewportBounds[1].y - viewportBounds[0].y);
		
			// Editor camera
			auto camera = core::SceneManager::GetSceneManager()->GetEditorCameraPtr();
			const glm::mat4& cameraProjection = camera->GetProjection();
			glm::mat4 cameraView = camera->GetViewMatrix();
			// Entity transform
			auto& tc = entity.GetComponent<core::TransformComponent>();
			glm::mat4 transform = tc.getTransform();
		
			if (keyboard->IsKeyPressed(core::KeyCode::KEY_1))
			{
				guizmoType = ImGuizmo::OPERATION::ROTATE;
			}
			else if (keyboard->IsKeyPressed(core::KeyCode::KEY_2))
			{
				guizmoType = ImGuizmo::OPERATION::SCALE;
			}
			else 
			{
				guizmoType = ImGuizmo::OPERATION::TRANSLATE;
			}
			// Snapping
			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)guizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform));
		
			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				DecomposeTransform(transform, translation, rotation, scale);
		
				glm::vec3 deltaRotation = rotation - tc.rotation;
				tc.translation = translation;
				tc.rotation += deltaRotation;
				tc.scale = scale;
			}
		
		}

		if (ImGui::IsWindowHovered() && !ImGuizmo::IsUsing())
		{
			auto sceneRef = core::SceneManager::GetSceneManager()->GetSceneRef();
			int pixel = -1;
			
			context->BindGBuffer();
			glReadBuffer(GL_COLOR_ATTACHMENT4);
			glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel);

			if (sceneRef->GetRegisterRef().valid(entt::entity(pixel)))
			{
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !keyboard->IsKeyPressed(core::KeyCode::KEY_LEFT_CONTROL))
				{
					sceneRef->SetSelectedEntityID(entt::entity(pixel));
				}
			}
			else
			{
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					sceneRef->ResetSelectedEntityID();
				}
			}

			context->UnbindGBuffer();
		}

		ImGui::End();
		ImGui::PopStyleVar();  
	}


	void Viewport::OnUpdate()
	{

	}

	glm::vec2 Viewport::GetWindowContentSize() const
	{
		return m_ViewportSize;
	}

}