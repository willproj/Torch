#include "Serializer.h"
#include <yaml-cpp/yaml.h>
#include <core/renderer/Entity.h>
#include <core/renderer/Component.h>
#include <core/renderer/ModelManager.h>
#include <core/renderer/SceneManager.h>
#include <core/glcore/Texture.h>

namespace YAML {

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace utils
{

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	static void SerializeEntity(YAML::Emitter& out, core::Entity entity)
	{
		out << YAML::BeginMap;

		if (entity.HasComponent<core::UUIDComponent>())
		{
			out << YAML::Key << "UUID Component";
			out << YAML::BeginMap;

			auto& uuidComp = entity.GetComponent<core::UUIDComponent>();
			out << YAML::Key << "Entity UUID" << YAML::Value << uuidComp.uuid.ToString();
			out << YAML::EndMap;
		}

		if (entity.HasComponent<core::EntityTypeComponent>())
		{
			out << YAML::Key << "Entity Type Component";
			out << YAML::BeginMap;

			auto& typeComp = entity.GetComponent<core::EntityTypeComponent>();
			out << YAML::Key << "Entity Type" << YAML::Value << core::GetEntityTypeStr(typeComp.entityType);
			out << YAML::EndMap;
		}

		if (entity.HasComponent<core::LabelComponent>())
		{
			out << YAML::Key << "Label Component";
			out << YAML::BeginMap;

			auto& labelComp = entity.GetComponent<core::LabelComponent>();
			out << YAML::Key << "Label" << YAML::Value << labelComp.label;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<core::TransformComponent>())
		{
			out << YAML::Key << "Transform Component";
			out << YAML::BeginMap;

			auto& transformComp = entity.GetComponent<core::TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << transformComp.translation;
			out << YAML::Key << "Rotation" << YAML::Value << transformComp.rotation;
			out << YAML::Key << "Scale" << YAML::Value << transformComp.scale;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<core::ModelComponent>())
		{
			out << YAML::Key << "Model Component";
			out << YAML::BeginMap;
			auto& modelComp = entity.GetComponent<core::ModelComponent>();
			out << YAML::Key << "Mesh Path" << YAML::Value << modelComp.model->GetModelPath();

			out << YAML::EndMap;
		}

		if (entity.HasComponent<core::MaterialComponent>())
		{
			out << YAML::Key << "Material Component";
			out << YAML::BeginMap;
			auto& materialComp = entity.GetComponent<core::MaterialComponent>();
			out << YAML::Key << "Albedo Path" << YAML::Value << materialComp.albedoPath;
			out << YAML::Key << "Normal Path" << YAML::Value << materialComp.normalPath;
			out << YAML::Key << "Metallic Path" << YAML::Value << materialComp.metallicPath;
			out << YAML::Key << "Roughness Path" << YAML::Value << materialComp.roughnessPath;
			out << YAML::Key << "AO Path" << YAML::Value << materialComp.aoPath;

			out << YAML::Key << "Metallic" << YAML::Value << materialComp.metallic;
			out << YAML::Key << "Roughness" << YAML::Value << materialComp.roughness;

			out << YAML::Key << "Use Albedo" << YAML::Value << materialComp.useAlbedoTexture;
			out << YAML::Key << "Use Normal" << YAML::Value << materialComp.useNormalTexture;
			out << YAML::Key << "Use Metallic" << YAML::Value << materialComp.useMetallicTexture;
			out << YAML::Key << "Use Roughness" << YAML::Value << materialComp.useRoughnessTexture;
			out << YAML::Key << "Use AO" << YAML::Value << materialComp.useAOTexture;
			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}
	Serializer::Serializer(core::Scene* scene)
		:m_ScenePtr(scene)
	{

	}
	void Serializer::Serialize(const std::string& path)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "unknow";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		auto& view = m_ScenePtr->GetRegisterRef().view<entt::entity>();
		for (auto& entityID : view)
		{
			core::Entity entity = { entityID, m_ScenePtr };
			if (entity.IsEntityEmpty())
			{
				return;
			}
			SerializeEntity(out, entity);
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fileOut(path);
		fileOut << out.c_str();
	}

	bool Serializer::DeSerialize(const std::string& path)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(path);
		}
		catch (YAML::ParserException e)
		{
			std::cout << "YAML failed to load .rx file" << std::endl;
			return false;
		}

		if (!data["Scene"])
		{
			return false;
		}

		auto entities = data["Entities"];
		core::Scene scene;

		if (entities)
		{
			for (auto entity : entities)
			{
				std::string label;
				auto labelComp = entity["Label Component"];
				if (labelComp)
				{
					label = labelComp["Label"].as<std::string>();
				}

				auto typeComp = entity["Entity Type Component"];
				core::EntityType type = core::EntityType::General;
				if (typeComp)
				{
					type = core::GetEntityType(typeComp["Entity Type"].as<std::string>());
				}

				core::Entity deserializedEntity = scene.CreateEntity(label, type);

				auto uuidComp = entity["UUID Component"];
				if (uuidComp)
				{
					auto& uuid = deserializedEntity.GetComponent<core::UUIDComponent>();
					uuid.uuid.SetUUIDStr(uuidComp["Entity UUID"].as<std::string>());
				}

				auto transformComp = entity["Transform Component"];
				if (transformComp)
				{
					auto& tansform = deserializedEntity.GetComponent<core::TransformComponent>();
					tansform.translation = transformComp["Translation"].as<glm::vec3>();
					tansform.rotation = transformComp["Rotation"].as<glm::vec3>();
					tansform.scale = transformComp["Scale"].as<glm::vec3>();
				}

				auto modelComp = entity["Model Component"];
				if (modelComp)
				{
					auto& model = deserializedEntity.GetComponent<core::ModelComponent>();
					std::string path = modelComp["Mesh Path"].as<std::string>();
					core::ModelManager::GetInstance()->LoadModel(path);
					model.model = core::ModelManager::GetInstance()->GetModel(path);
				}

				auto materialComp = entity["Material Component"];
				if (materialComp)
				{
					deserializedEntity.AddComponent<core::MaterialComponent>();
					auto& material = deserializedEntity.GetComponent<core::MaterialComponent>();
					material.albedoPath = materialComp["Albedo Path"].as<std::string>();
					if (!material.albedoPath.empty())
					{
						material.albedoTexture = core::Texture::LoadTexture(material.albedoPath);
					}

					material.normalPath = materialComp["Normal Path"].as<std::string>();
					if (!material.normalPath.empty())
					{
						material.normalTexture = core::Texture::LoadTexture(material.normalPath);
					}
					
					material.metallicPath = materialComp["Metallic Path"].as<std::string>();
					if (!material.metallicPath.empty())
					{
						material.metallicTexture = core::Texture::LoadTexture(material.metallicPath);
					}
					
					material.roughnessPath = materialComp["Roughness Path"].as<std::string>();
					if (!material.roughnessPath.empty())
					{
						material.roughnessTexture = core::Texture::LoadTexture(material.roughnessPath);
					}
					
					material.aoPath = materialComp["AO Path"].as<std::string>();
					if (!material.aoPath.empty())
					{
						material.aoTexture = core::Texture::LoadTexture(material.aoPath);
					}
					

					material.useAlbedoTexture = materialComp["Use Albedo"].as<bool>();
					material.useNormalTexture = materialComp["Use Normal"].as<bool>();
					material.useMetallicTexture = materialComp["Use Metallic"].as<bool>();
					material.useRoughnessTexture = materialComp["Use Roughness"].as<bool>();
					material.useAOTexture = materialComp["Use AO"].as<bool>();

					material.metallic = materialComp["Metallic"].as<float>();
					material.roughness = materialComp["Roughness"].as<float>();

				}
			}
		}
		core::SceneManager::GetSceneManager()->SetScene(std::move(scene));
		return true;
	}
}