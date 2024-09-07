#pragma once
#include <pch/pch.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <utils/UUID.h>
#include "core/renderer/camera/EditorCamera.h"
#include "core/glcore/TorchModel.h"

namespace core
{
	enum class EntityType
	{
		General,
		Light
	};

	static std::string GetEntityTypeStr(EntityType type)
	{
		switch (type)
		{
		case EntityType::General:
			return "General";
		case EntityType::Light:
			return "Light";
		default:
			return "Unknow";
		}
	}

	static EntityType GetEntityType(const std::string& type)
	{
		if (type == "Light")
		{
			return EntityType::Light;
		}
		return EntityType::General;
	}

	struct UUIDComponent
	{
		utils::UUID uuid;
		UUIDComponent() = default;
		UUIDComponent(const UUIDComponent&) = default;
		UUIDComponent& operator=(const UUIDComponent&) = default;
	};

	struct LabelComponent
	{
		std::string label;

		LabelComponent() = default;
		LabelComponent(const LabelComponent&) = default;
		LabelComponent& operator=(const LabelComponent&) = default;

		LabelComponent(const std::string& label) :label(label) {}
	};

	struct EntityTypeComponent
	{
		EntityType entityType; // Changed the member name to avoid conflict

		EntityTypeComponent() = default;
		EntityTypeComponent(const EntityTypeComponent&) = default;
		EntityTypeComponent& operator=(const EntityTypeComponent&) = default;

		EntityTypeComponent(const EntityType& type)
			: entityType(type) // Changed this to refer to the correct member
		{}
	};


	struct TransformComponent
	{
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
		glm::vec3 translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent& operator=(const TransformComponent&) = default;

		glm::mat4 getTransform() const
		{

			glm::mat4 rotationMat = glm::toMat4(glm::quat(rotation));

			return glm::translate(glm::mat4(1.0f), translation)
				* rotationMat
				* glm::scale(glm::mat4(1.0f), scale);
		}
	};

	struct CameraComponent
	{
		EditorCamera editorCamera;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
		CameraComponent& operator=(const CameraComponent&) = default;
	};


	struct ColorComponent
	{
		glm::vec3 color;

		ColorComponent() = default;
		ColorComponent(const ColorComponent&) = default;
		ColorComponent& operator=(const ColorComponent&) = default;
	};


	struct ModelComponent
	{
		std::shared_ptr<TorchModel> model;
		ModelComponent() = default;
		ModelComponent(const ModelComponent&) = default;
		ModelComponent& operator=(const ModelComponent&) = default;
	};


	struct MaterialComponent
	{
		uint32_t albedoTexture = 0;
		uint32_t normalTexture = 0;
		uint32_t metallicTexture = 0;
		uint32_t roughnessTexture = 0;
		uint32_t aoTexture = 0;

		std::string albedoPath;
		std::string normalPath;
		std::string metallicPath;
		std::string roughnessPath;
		std::string aoPath;

		bool useAlbedoTexture = true;
		bool useNormalTexture = true;
		bool useMetallicTexture = true;
		bool useRoughnessTexture = true;
		bool useAOTexture = true;

		float roughness = 0.6;
		float metallic = 0;

		MaterialComponent() = default;
		MaterialComponent(const MaterialComponent&) = default;
		MaterialComponent& operator=(const MaterialComponent&) = default;
	};
}