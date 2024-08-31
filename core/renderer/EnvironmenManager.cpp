#include "EnvironmentManager.h"

namespace core
{
    std::shared_ptr<EnvironmentManager> EnvironmentManager::m_Instance = nullptr;

    std::shared_ptr<EnvironmentManager> EnvironmentManager::GetInstance()
    {
        if (!m_Instance)
        {
            m_Instance = std::shared_ptr<EnvironmentManager>(new EnvironmentManager());
        }
        return m_Instance;
    }
    void EnvironmentManager::RemoveEntity(const EnvironmentEntityType& type)
    {
        m_EnvironmentEntitiesMap.erase(type);
    }
    const EnvironmentEntity& EnvironmentManager::GetEnvironmentEntity(const EnvironmentEntityType& type) const
    {
        auto it = m_EnvironmentEntitiesMap.find(type);
        if (it != m_EnvironmentEntitiesMap.end())
        {
            // Return a reference to the concrete type object managed by std::shared_ptr
            return *(it->second);
        }
        throw std::out_of_range("Entity not found");
    }

    void EnvironmentManager::AddEntity(EnvironmentEntityType type, std::shared_ptr<EnvironmentEntity> entity)
    {
        m_EnvironmentEntitiesMap[type] = std::move(entity);
    }

    std::shared_ptr<EnvironmentEntity> EnvironmentManager::GetEnvironmentEntityPtr(const EnvironmentEntityType& type)
    {
        auto it = m_EnvironmentEntitiesMap.find(type);
        if (it != m_EnvironmentEntitiesMap.end())
        {
            return it->second;
        }
        return nullptr;
    }

    EnvironmentManager::EnvironmentManager()
    {
    }
}