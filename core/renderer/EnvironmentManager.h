#pragma once 
#include <pch/pch.h>
#include "environment/AtmosphericScattering.h"
#include "camera/EditorCamera.h"

namespace core
{
    class EnvironmentManager
    {
    public:
        static std::shared_ptr<EnvironmentManager> GetInstance();
        ~EnvironmentManager() = default;

        void AddEntity(EnvironmentEntityType type, std::shared_ptr<EnvironmentEntity> entity);
        void RemoveEntity(const EnvironmentEntityType& type);
        const EnvironmentEntity& GetEnvironmentEntity(const EnvironmentEntityType& type) const;
        std::shared_ptr<EnvironmentEntity> GetEnvironmentEntityPtr(const EnvironmentEntityType& type);
    private:
        EnvironmentManager();
        static std::shared_ptr<EnvironmentManager> m_Instance;
        std::unordered_map<EnvironmentEntityType, std::shared_ptr<EnvironmentEntity>> m_EnvironmentEntitiesMap;
    };

}