#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include "Primitives.h"
#include "core/System.h"

class SModelManager : public System<SModelManager> {
public:
    std::shared_ptr<Model> LoadModel(const std::string& model_name) {
        // Check if model is already loaded
        auto it = m_models.find(model_name);
        if (it != m_models.end()) {
            return it->second;
        }

        // Create and load new model
        auto model = std::make_shared<Model>();
        if (model->LoadMeshFromFile(model_name.c_str())) {
            m_models[model_name] = model;
            return model;
        }
        return nullptr;
    }

    void UnloadModel(const std::string& fileName) {
        m_models.erase(fileName);
    }

private:
    std::unordered_map<std::string, std::shared_ptr<Model>> m_models;
};