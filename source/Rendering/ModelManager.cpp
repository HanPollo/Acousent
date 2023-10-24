#include "ModelManager.h"

ModelManager& ModelManager::getInstance()
{
	static ModelManager instance; // Guaranteed to be destroyed.
	return instance;
}

void ModelManager::addModel(const std::string& modelName, std::shared_ptr<Model> model)
{
	models[modelName] = model;
}

std::shared_ptr<Model> ModelManager::getModel(const std::string& modelName)
{
	if (modelExists(modelName)) {
		return models[modelName];
	}
	else return models["Synth"];
}

bool ModelManager::modelExists(const std::string& modelName)
{
	auto it = models.find(modelName);
	if (it != models.end()) {
		// Model with the same name already exists. Handle it as you wish.
		// You can notify, throw an exception, or just return false.
		return true;
	}
	else return false;
}
