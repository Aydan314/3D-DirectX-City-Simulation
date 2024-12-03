#include "JSONmanager.h"

JSONmanager::JSONmanager()
{
	_loadedValues = LoadObjectsFromJSON("JSON/GameObjects.json");
}

std::vector<GameObjectValues> JSONmanager::LoadObjectsFromJSON(std::string filename)
{
	std::vector<GameObjectValues> loadedValues;

	json JSONfile;

	std::ifstream file(filename);

	JSONfile = json::parse(file);

	std::string version = JSONfile["version"].get<std::string>();

	json& gameObjects = JSONfile["GameObjects"];

	int size = gameObjects.size();
	for (unsigned int i = 0; i < size; i++) 
	{
		// Extract object data //
		GameObjectValues objectValues;
		json& objectDesc = gameObjects.at(i);

		// Extract values //
		objectValues.name = objectDesc["Name"];
		objectValues.meshID = objectDesc["MeshID"];
		objectValues.specularTextureID = objectDesc["SpecularTextureID"];
		objectValues.textureID = objectDesc["TextureID"];
		objectValues.collisionEnabled = objectDesc["CollisionEnabled"];

		// Extract Hitbox //
		json& hitboxDesc = objectDesc["Hitbox"];

		objectValues.hitbox.minPosition.x = hitboxDesc["MinX"];
		objectValues.hitbox.minPosition.y = hitboxDesc["MinY"];
		objectValues.hitbox.minPosition.z = hitboxDesc["MinZ"];
		objectValues.hitbox.maxPosition.x = hitboxDesc["MaxX"];
		objectValues.hitbox.maxPosition.y = hitboxDesc["MaxY"];
		objectValues.hitbox.maxPosition.z = hitboxDesc["MaxZ"];

		// Extract position //
		json& positionDesc = objectDesc["Position"];

		objectValues.transform.position.x = positionDesc["x"];
		objectValues.transform.position.y = positionDesc["y"];
		objectValues.transform.position.z = positionDesc["z"];

		// Extract rotation //
		json& rotationDesc = objectDesc["Rotation"];

		objectValues.transform.rotation.x = rotationDesc["x"];
		objectValues.transform.rotation.y = rotationDesc["y"];
		objectValues.transform.rotation.z = rotationDesc["z"];

		// Extract scale //
		json& scaleDesc = objectDesc["Scale"];

		objectValues.transform.scale.x = scaleDesc["x"];
		objectValues.transform.scale.y = scaleDesc["y"];
		objectValues.transform.scale.z = scaleDesc["z"];

		loadedValues.push_back(objectValues);
	}

	file.close();

	return loadedValues;
}

GameObjectValues* JSONmanager::GetGameObject(GameObjectID ID)
{
	return &_loadedValues[ID];
}
