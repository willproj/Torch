#pragma once
#include <pch/pch.h>
#include "core/renderer/camera/EditorCamera.h"
namespace core
{
	enum class EnvironmentEntityType
	{
		None,
		Atmosphere,
		VolmetricFog,
	};


	struct AtmosphericScatteringSpecification
	{
		glm::vec3 sunDir = glm::vec3(1.0f);
	};

	using SpecificationVariant = std::variant<AtmosphericScatteringSpecification>;
	
	class EnvironmentEntity
	{
	public:
		EnvironmentEntity() = default;
		virtual ~EnvironmentEntity() = default;

		virtual void Render() = 0;
		virtual void Initialize() = 0;
		virtual utils::Ref<SpecificationVariant> GetSpecification() = 0;
		virtual EnvironmentEntityType GetType() const { return m_Type; }
	protected:
		EnvironmentEntityType m_Type = EnvironmentEntityType::None;
		std::shared_ptr<EditorCamera> m_CurrentCamera= nullptr;
	};
}