#include "ModelManager.h"

namespace core
{
    std::shared_ptr<ModelManager> ModelManager::s_ModelManager = nullptr;
    std::shared_ptr<ModelManager> ModelManager::GetInstance()
    {
        if (s_ModelManager == nullptr)
        {
            s_ModelManager = std::make_shared<ModelManager>();
        }
        return s_ModelManager;
    }

    std::shared_ptr<TorchModel> ModelManager::LoadModel(const std::string &modelPath)
    {
        // Check if model is already loaded
        if (m_Models.find(modelPath) != m_Models.end())
        {
            return m_Models[modelPath];
        }

        // Load and store new model
        auto model = std::make_shared<TorchModel>();
        model->LoadModel(modelPath);
        m_Models[modelPath] = model;
        return model;
    }

    std::shared_ptr<TorchModel> ModelManager::GetModel(const std::string &modelPath)
    {
        auto it = m_Models.find(modelPath);
        if (it != m_Models.end())
        {
            return it->second;
        }
        return nullptr; // or throw an exception if preferred
    }

    void ModelManager::RenderModel(const std::string &modelPath)
    {
        auto model = GetModel(modelPath);
        if (model)
        {
            model->RenderModel();
        }
        else
        {
            std::cerr << "Model not found: " << modelPath << std::endl;
        }
    }
}