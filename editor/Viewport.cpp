#include "Viewport.h"
#include <utils/ServiceLocator.h>

namespace editor
{
	void Viewport::Render()
	{
		auto context = utils::ServiceLocator::GetGraphicsContext();
		ImGui::Begin("viewport");
		ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(context->GetScreenTexture())), ImVec2(1280, 720));
		ImGui::End();
	}

	void Viewport::OnUpdate()
	{

	}

}