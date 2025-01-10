#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include "Primitives.h"

class ModelManager {
private:
    std::unordered_map<std::string, std::shared_ptr<Model>> models;

public:
    std::shared_ptr<Model> LoadModel(const std::string& fileName, const Vector3& color) {
        // Check if model is already loaded
        auto it = models.find(fileName);
        if (it != models.end()) {
            return it->second;
        }

        // Create and load new model
        auto model = std::make_shared<Model>();
        if (model->LoadMeshFromFile(fileName.c_str(), color)) {
            models[fileName] = model;
            return model;
        }
        return nullptr;
    }

    void UnloadModel(const std::string& fileName) {
        models.erase(fileName);
    }
};