#include "EnvironmentPropertiesPanel.h"

namespace editor
{
	void EnvironmentPropertiesPanel::Render()
	{
		// Set up environment manager
		m_EnvirManager = core::EnvironmentManager::GetInstance();

		// Render Gui
		ImGui::Begin("Environment Setting Panel");

		RenderAtmosphereHeader();

		ImGui::End();
	}

	void EnvironmentPropertiesPanel::OnUpdate()
	{

	}

	void EnvironmentPropertiesPanel::RenderAtmosphereHeader()
	{
		auto atmosphere = m_EnvirManager->GetEnvironmentEntityPtr(core::EnvironmentEntityType::Atmosphere);
		auto& atmosSpecific = std::get<core::AtmosphericScatteringSpecification>(atmosphere->GetSpecification().get());
		if (ImGui::CollapsingHeader("Atmospheric Scattering", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// Create the color picker in ImGui
			ImGui::Text("Sun Color");
			ImGui::ColorEdit3(" ", glm::value_ptr(atmosSpecific.sunColor), ImGuiColorEditFlags_Float);
			ImGui::Text("Sun Intensity");
			ImGui::SliderFloat(" ", &atmosSpecific.sunIntensity, 0.1f, 10.0f, "%.1f");

			// Define a variable to hold the sun angle in degrees
			static float sunAngleDegrees = 0.0f; // Default value in degrees

			// Create the slider in ImGui
			ImGui::Text("Sun Angle (Degrees)");
			ImGui::SliderFloat("##Sun Angle", &sunAngleDegrees, 0.0f,5.0f, "%.2f");

			// Convert the angle from degrees to radians
			atmosSpecific.sunAngle = sunAngleDegrees * (M_PI / 180.0f);


			// Sun Position in spherical coordinates
			static float azimuthalAngle = 0.25f; // ¦È (rotation around Z-axis)
			static float polarAngle = 0.0f; // ¦Õ (elevation angle from Z-axis)

			// Convert spherical coordinates to Cartesian coordinates
			ImGui::Text("Sun Position (Spherical Coordinates)");
			// Azimuthal angle (¦È) - Rotation around Z-axis
			ImGui::Text("Azimuthal Angle");
			ImGui::SliderFloat("##Azimuthal Angle", &azimuthalAngle, 0.0f, 2.0f * M_PI, "%.2f");
			// Polar angle (¦Õ) - Elevation angle from the Z-axis
			ImGui::Text("Polar Angle");
			ImGui::SliderFloat("##Polar Angle", &polarAngle, -M_PI / 2.0f, M_PI / 2.0f, "%.2f");

			if (m_LastAzimuthalAngle != azimuthalAngle || m_LastPolarAngle != polarAngle)
			{
				atmosphere->SetIsRunning(true);
				m_LastAzimuthalAngle = azimuthalAngle;
				m_LastPolarAngle = polarAngle;
			}
			else
			{
				atmosphere->SetIsRunning(false);
			}

			// Update sun position based on slider values
			float x = cos(polarAngle) * cos(azimuthalAngle);
			float y = cos(polarAngle) * sin(azimuthalAngle);
			float z = sin(polarAngle);
			atmosSpecific.sunPosition = glm::normalize(glm::vec3(x, y, z));

			// Ray Minimum Distance
			ImGui::Text("Ray Minimum Distance");
			ImGui::SliderFloat("##Ray Minimum Distance", &atmosSpecific.rayTMin, 0.0001f, 0.01f, "%.4f");
			ImGui::NewLine();
			ImGui::Separator();
			
			// Earth Parameters
			ImGui::Text("Earth Parameters");
			ImGui::Text("Earth Radius"); 
			ImGui::SliderFloat("##Earth", &atmosSpecific.earthRadius, 6350000.0f, 6400000.0f, "%.1f");

			ImGui::Text("Atmosphere Height"); 
			ImGui::SliderFloat("##Atmosphere", &atmosSpecific.atmosphereHeight, 90000.0f, 120000.0f, "%.1f");
			ImGui::NewLine();
			ImGui::Separator();

			// Heights
			ImGui::Text("Heights");
			ImGui::Text("Rayleigh Height");
			ImGui::SliderFloat("##Rayleigh Height", &atmosSpecific.rayleighHeight, 6000.0f, 10000.0f, "%.1f");

			ImGui::Text("Mie Height"); 
			ImGui::SliderFloat("##Mie Height", &atmosSpecific.mieHeight, 1000.0f, 3500.0f, "%.1f");
			ImGui::NewLine();
			ImGui::Separator();


			// Scattering Coefficients
			ImGui::Text("Rayleigh Scattering Coefficient");
			// Red
			ImGui::Text("R"); ImGui::SameLine(); 
			ImGui::SliderFloat("##RayleighR", &atmosSpecific.rayleighScatteringCoef.r, 1e-7f, 5e-5f, "%.7f");

			// Green
			ImGui::Text("G"); ImGui::SameLine();
			ImGui::SliderFloat("##RayleighG", &atmosSpecific.rayleighScatteringCoef.g, 1e-7f, 5e-5f, "%.7f");

			// Blue
			ImGui::Text("B"); ImGui::SameLine(); 
			ImGui::SliderFloat("##RayleighB", &atmosSpecific.rayleighScatteringCoef.b, 1e-7f, 5e-5f, "%.7f");

			ImGui::Text("Mie Scattering Coefficient");

			// Red
			ImGui::Text("R"); ImGui::SameLine(); 
			ImGui::SliderFloat("##MieR", &atmosSpecific.mieScatteringCoef.r, 1e-7f, 5e-5f, "%.7f");

			// Green
			ImGui::Text("G"); ImGui::SameLine(); 
			ImGui::SliderFloat("##MieG", &atmosSpecific.mieScatteringCoef.g, 1e-7f, 5e-5f, "%.7f");

			// Blue
			ImGui::Text("B"); ImGui::SameLine(); 
			ImGui::SliderFloat("##MieB", &atmosSpecific.mieScatteringCoef.b, 1e-7f, 5e-5f, "%.7f");

			ImGui::Text("Ozone Absorption Coefficient");

			// Red
			ImGui::Text("R"); ImGui::SameLine(); 
			ImGui::SliderFloat("##OzoneR", &atmosSpecific.ozoneAbsorptionCoef.r, 1e-6f, 1e-4f, "%.7f");

			// Green
			ImGui::Text("G"); ImGui::SameLine(); 
			ImGui::SliderFloat("##OzoneG", &atmosSpecific.ozoneAbsorptionCoef.g, 1e-6f, 1e-4f, "%.7f");

			// Blue
			ImGui::Text("B"); ImGui::SameLine();
			ImGui::SliderFloat("##OzoneB", &atmosSpecific.ozoneAbsorptionCoef.b, 1e-6f, 1e-4f, "%.7f");


		}
	}
}