#pragma once
#include <pch/pch.h>
#include <core/renderer/Scene.h>

namespace utils
{
    class Serializer
    {
    public:
        Serializer(core::Scene* scene);
        ~Serializer() = default;

        void Serialize(const std::string& path);
        bool DeSerialize(const std::string& path);

    private:
        core::Scene* m_ScenePtr;
    };
}