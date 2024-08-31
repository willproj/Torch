#pragma once
#include <pch/pch.h>
#include "core/glcore/TorchModel.h"

namespace core
{
    class ModelManager
    {
    public:
        static std::shared_ptr<ModelManager> GetInstance();
        std::shared_ptr<TorchModel> LoadModel(const std::string &modelPath);
        std::shared_ptr<TorchModel> GetModel(const std::string &modelPath);
        void RenderModel(const std::string &modelPath);

        ModelManager(const ModelManager &) = delete;
        ModelManager &operator=(const ModelManager &) = delete;
        ~ModelManager() = default;
        ModelManager() = default;
    private:
        static std::shared_ptr<ModelManager> s_ModelManager;
        std::unordered_map<std::string, std::shared_ptr<TorchModel>> m_Models;
    };
}