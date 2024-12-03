#pragma once
#include "GameDefinitions.h"
#include "JSONmanager.h"

class WorldGenerator
{
private:
	JSONmanager* _jsonManager;
	std::vector<std::vector<GameObjectValues>> _worldTiles;
	float _tileSpacing = 15.f;

public:
	WorldGenerator(JSONmanager* jsonManager);

	std::vector<std::vector<GameObjectValues>> GenerateWorld(XMINT2 worldSize, int seed);
	void GenerateFloor(XMINT2 worldSize);
	void GenerateBuildings();
	void GenerateRoads(XMINT2 worldSize);
	std::vector<GameObjectValues> GenerateCars();
	std::vector<XMFLOAT3> GenerateCarRoute(Transform carTransform);
	std::vector<LightValues> GenerateLampPosts();
	XMFLOAT3 ChooseIntersection(XMFLOAT3 prevChoice, std::vector<XMFLOAT3> currentRoute, bool row);
	bool IsEqual(XMFLOAT3 A, XMFLOAT3 B);

	void SetTileSpacing(float spacing);
	float GetTileSpacing();
};

