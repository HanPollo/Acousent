#include <map>
#include <string>
#include <memory> // for smart pointers

#include "Model.h"

// Singleton class that manages all models
class ModelManager {
public:
    // Gets the single instance of the class
    static ModelManager& getInstance();

    // Add a model to the manager
    void addModel(const std::string& modelName, std::shared_ptr<Model> model);

    // Get a model by name
    std::shared_ptr<Model> getModel(const std::string& modelName);

    // Check if a model exists
    bool modelExists(const std::string& modelName);

private:
    ModelManager() {}  // Private so that it can't be called
    std::map<std::string, std::shared_ptr<Model>> models;

    // C++ 11
    // Stop the compiler from generating copy methods for your singleton
    ModelManager(ModelManager const&) = delete;
    void operator=(ModelManager const&) = delete;
};