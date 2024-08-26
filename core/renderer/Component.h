#pragma once
#include <pch/pch.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <utils/UUID.h>
#include "core/renderer/camera/EditorCamera.h"
#include "core/glcore/TorchModel.h"

namespace core
{
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

}