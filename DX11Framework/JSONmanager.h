#pragma once
#include "GameDefinitions.h"

class JSONmanager
{
private:
	std::vector<GameObjectValues> _loadedValues;

public:
	JSONmanager();

	std::vector<GameObjectValues> LoadObjectsFromJSON(std::string filename);
	GameObjectValues* GetGameObject(GameObjectID ID);
};

