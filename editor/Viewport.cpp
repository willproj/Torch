#include "Viewport.h"
#include <utils/ServiceLocator.h>

namespace editor
{
	void Viewport::Render()
	{
		auto context = utils::ServiceLocator::GetGraphicsContext();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f)); 
		ImGui::Begin("viewport");

		m_ViewportSize = glm::vec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y); 

		ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(context->GetScreenTexture())),
			ImVec2(m_ViewportSize.x, m_ViewportSize.y),
			ImVec2(0, 1),
			ImVec2(1, 0));


		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		viewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		viewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
		
		
		auto [x, y] = ImGui::GetMousePos();
		x -= viewportBounds[0].x;
		y -= viewportBounds[0].y;
		y = m_ViewportSize.y - y;
		x *= utils::ServiceLocator::GetWindow()->GetWinSpecification().width * 1.0f / m_ViewportSize.x;
		y *= utils::ServiceLocator::GetWindow()->GetWinSpecification().height * 1.0f / m_ViewportSize.y;

		if (ImGui::IsWindowHovered())
		{
			context->BindGBuffer();

			//color attachment 3 = red int
			glReadBuffer(GL_COLOR_ATTACHMENT3);
			int pixel = -1;
			glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel);
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