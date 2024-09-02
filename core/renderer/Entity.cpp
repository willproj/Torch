#include "Entity.h"

namespace core
{
	Entity::Entity(entt::entity entity, Scene* scene)
		:m_Entity(entity), m_ScenePtr(scene)
	{
		m_Type = EntityType::General;
	}

	Entity::Entity(entt::entity entity, Scene* scene, EntityType type)
		:m_Entity(entity), m_ScenePtr(scene), m_Type(type)
	{

	}

	/*AABB generateAABB(const tinygltf::Model& model)
	{
		glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
		glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

		for (const auto& mesh : model.meshes)
		{
			for (const auto& primitive : mesh.primitives)
			{
				const auto& positionAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
				const auto& bufferView = model.bufferViews[positionAccessor.bufferView];
				const auto& buffer = model.buffers[bufferView.buffer];

				const float* positions = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + positionAccessor.byteOffset]);
				for (size_t i = 0; i < positionAccessor.count; ++i)
				{
					glm::vec3 position(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2]);

					minAABB.x = std::min(minAABB.x, position.x);
					minAABB.y = std::min(minAABB.y, position.y);
					minAABB.z = std::min(minAABB.z, position.z);

					maxAABB.x = std::max(maxAABB.x, position.x);
					maxAABB.y = std::max(maxAABB.y, position.y);
					maxAABB.z = std::max(maxAABB.z, position.z);
				}
			}
		}

		return AABB(minAABB, maxAABB);
	}

	Sphere generateSphereBV(const tinygltf::Model& model)
	{
		glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
		glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());

		for (const auto& mesh : model.meshes)
		{
			for (const auto& primitive : mesh.primitives)
			{
				const auto& positionAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
				const auto& bufferView = model.bufferViews[positionAccessor.bufferView];
				const auto& buffer = model.buffers[bufferView.buffer];

				const float* positions = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + positionAccessor.byteOffset]);
				for (size_t i = 0; i < positionAccessor.count; ++i)
				{
					glm::vec3 position(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2]);

					minAABB.x = std::min(minAABB.x, position.x);
					minAABB.y = std::min(minAABB.y, position.y);
					minAABB.z = std::min(minAABB.z, position.z);

					maxAABB.x = std::max(maxAABB.x, position.x);
					maxAABB.y = std::max(maxAABB.y, position.y);
					maxAABB.z = std::max(maxAABB.z, position.z);
				}
			}
		}

		glm::vec3 center = (maxAABB + minAABB) * 0.5f;
		float radius = glm::length(maxAABB - center);

		return Sphere(center, radius);
	}*/

}