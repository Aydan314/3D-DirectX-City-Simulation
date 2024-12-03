#pragma once
#include "GameObject.h"
#include "PointLight.h"

class GameScene
{
private:
	std::vector<GameObject*> _scene;
	std::vector<PointLight*> _lights;

public:
	GameScene();

	void AddGameObject(GameObject* newObject);
	void AddNewLight(PointLight* newLight);

	void Update(float deltaTime);
	void Draw(FrameData frameData);

	GameObject* GetObjectAtIndex(int index);
	GameObject* GetFirstInstanceOfName(std::string name);

	PointLight* GetLightAtIndex(int index);
	std::vector<GameObject*> GetAllObjects();
};

