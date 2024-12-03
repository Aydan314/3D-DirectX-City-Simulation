#include "GameScene.h"

GameScene::GameScene()
{
	_scene = {};
}

void GameScene::AddGameObject(GameObject* newObject)
{
	_scene.push_back(newObject);
}

void GameScene::AddNewLight(PointLight* newLight)
{
	if (_lights.size() <= MAX_LIGHTS) _lights.push_back(newLight);
}

void GameScene::Update(float deltaTime)
{
	for (auto item : _scene) 
	{
		item->Update(deltaTime);
	}
}

void GameScene::Draw(FrameData frameData)
{
	for (auto item : _scene) 
	{
		item->Draw(frameData);
	}
}

GameObject* GameScene::GetObjectAtIndex(int index)
{
	if (index < _scene.size()) 
	{
		return _scene[index];
	}
	return nullptr;
}

GameObject* GameScene::GetFirstInstanceOfName(std::string name)
{
	for (GameObject* item : _scene) 
	{
		if (item->GetName() == name) 
		{
			return item;
		}
	}
	return nullptr;
}

PointLight* GameScene::GetLightAtIndex(int index)
{
	if (index < _lights.size()) return _lights[index];

	return nullptr;
}

std::vector<GameObject*> GameScene::GetAllObjects()
{
	return _scene;
}
