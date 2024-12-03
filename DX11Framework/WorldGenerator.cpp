#include "WorldGenerator.h"

WorldGenerator::WorldGenerator(JSONmanager* jsonManager)
{
    _jsonManager = jsonManager;
}

std::vector<std::vector<GameObjectValues>> WorldGenerator::GenerateWorld(XMINT2 worldSize, int seed)
{
    srand(seed);

    GenerateFloor(worldSize);
    GenerateBuildings();
    GenerateRoads(worldSize);

    return _worldTiles;
}

void WorldGenerator::GenerateFloor(XMINT2 worldSize)
{
    // Creates a flat platform of grass to the given world size //
    _worldTiles = {};

    for (int x = 0; x < worldSize.x; x++) 
    {
        std::vector<GameObjectValues> column;

        for (int y = 0; y < worldSize.y; y++) 
        {
            GameObjectValues objectValues = *_jsonManager->GetGameObject(OBJECT_GRASS_TILE);
            objectValues.transform.position.x = x * _tileSpacing;
            objectValues.transform.position.z = y * _tileSpacing;

            column.push_back(objectValues);
        }

        _worldTiles.push_back(column);
    }
}

void WorldGenerator::GenerateBuildings()
{
    // Randomly fill the platform with various buildings //

    for (int x = 0; x < _worldTiles.size(); x++) 
    {
        for (int y = 0; y < _worldTiles[0].size(); y++)
        {
            GameObjectValues objectValues = *_jsonManager->GetGameObject(GAME_BUILDINGS[rand() % GAME_BUILDINGS.size()]);
            objectValues.transform.position.x = x * _tileSpacing;
            objectValues.transform.position.z = y * _tileSpacing;

            _worldTiles[x][y] = objectValues;

        }
    }
}

void WorldGenerator::GenerateRoads(XMINT2 worldSize)
{
    // Generate randomly spaced roads along the platform X axis //
    int roadColumn = 0;
    while (roadColumn < worldSize.x)
    {
        for (int y = 0; y < worldSize.y; y++)
        {
            GameObjectValues objectValues = *_jsonManager->GetGameObject(OBJECT_ROAD_STRAIGHT);
            objectValues.transform.position.x = roadColumn * _tileSpacing;
            objectValues.transform.position.z = y * _tileSpacing;

            _worldTiles[roadColumn][y] = objectValues;
        }


        roadColumn += 3 + rand() % 4;
    }

    // Generate randomly spaced roads along the platform Y axis //
    int roadRow = 0;
    while (roadRow < worldSize.y)
    {
        for (int x = 0; x < worldSize.x; x++)
        {
            GameObjectValues objectValues = *_jsonManager->GetGameObject(OBJECT_ROAD_STRAIGHT);

            // If X and Y axis roads intersect place a crossroad // 
            if (_worldTiles[x][roadRow].name == "Road Straight") objectValues = *_jsonManager->GetGameObject(OBJECT_ROAD_CROSSROAD);

            objectValues.transform.rotation.y = XM_PI / 2.f;
            objectValues.transform.position.x = x * _tileSpacing;
            objectValues.transform.position.z = roadRow * _tileSpacing;

            _worldTiles[x][roadRow] = objectValues;
        }


        roadRow += 3 + rand() % 4;
    }
}

std::vector<GameObjectValues> WorldGenerator::GenerateCars()
{
    int carChance = 4;
    std::vector<GameObjectValues> cars;

    // Iterate through the map and look for road tiles //
    for (auto row : _worldTiles) 
    {
        for (auto tile : row) 
        {
            // If there is a road tile randomly choose if it should have a car on it //
            if (tile.name == "Road Straight" && rand() % carChance == 0) 
            {
                GameObjectValues objectValues = *_jsonManager->GetGameObject(OBJECT_CAR);
                objectValues.transform.position = { tile.transform.position.x,tile.transform.position.y + 0.2f,tile.transform.position.z};
                objectValues.transform.rotation = tile.transform.rotation;

                cars.push_back(objectValues);
            }
        }
    }

    return cars;
}

std::vector<XMFLOAT3> WorldGenerator::GenerateCarRoute(Transform carTransform)
{
    // Calculate position in the 2D array the car is //
    int startX = round(carTransform.position.x / _tileSpacing);
    int startY = round(carTransform.position.z / _tileSpacing);

    std::vector<XMFLOAT3> route;
    std::vector<XMFLOAT3> directionChoice;
    XMFLOAT3 prevPos = carTransform.position;
    XMFLOAT3 currentPos;
    int iteration = 0;

    // Check which axis the car should travel along first //
    bool rowOrColumn = carTransform.rotation.y != 0;

    bool done = false;
    while (!done) 
    {
        // select a random intersection on the current road to navigate to //
         currentPos = ChooseIntersection(prevPos, route, rowOrColumn);
         route.push_back(currentPos);

         // Check if route has completed a full loop //
         if (iteration > 1 && IsEqual(currentPos,route[0]))
         {
             done = true;
             route.push_back(currentPos);
         }

         prevPos = currentPos;
         iteration++;

         // Failsafe to stop route from generating too long //
         if (iteration > 40) 
         {
             done = true;
         }
        

         rowOrColumn = !rowOrColumn;
    }

    return route;
}

std::vector<LightValues> WorldGenerator::GenerateLampPosts()
{
    int lampPostChance = 16;
    float lamppostChanceMultiplier = 4.f;
    float lamppostUnlikelyhood = 1;
    float lightHeight = 3;
    float pavementAdjustment = 6;

    std::vector<LightValues> lampPosts;
    
    // Define how the light emitted by the streetlights will look //
    LightValues lightValues =
    {
        XMFLOAT4{ 0.2f,0.2f,0.0f,1.0f },
        XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f },

        XMFLOAT4{ 0.2f,0.2f,0.0f,1.0f },
        XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f },

        XMFLOAT4{ 0.2f,0.2f,0.0f,1.0f },
        XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f },
        200.0f,

        XMFLOAT3{ 0.0f,0.0f,0.0f },
        20.f,
        XMFLOAT3{0,1,0}
    };

    while (lampPosts.size() <= MAX_LIGHTS)
    {
        // Iterate through the whole map //
        for (int x = 0; x < _worldTiles.size(); x++) 
        {
            for (int y = 0; y < _worldTiles[0].size(); y++) 
            {
                // If a road is found choose to place a streetlight or not //
                if (_worldTiles[x][y].name == "Road Straight") 
                {
                    if (rand() % int(round(lampPostChance * lamppostUnlikelyhood)) == 0) 
                    {
                        XMFLOAT3 pos = _worldTiles[x][y].transform.position;

                        pos.y += lightHeight;
                        if (_worldTiles[x][y].transform.rotation.y != 0) pos.z += pavementAdjustment;
                        else pos.x += pavementAdjustment;

                        lightValues.lightPos = pos;

                        lampPosts.push_back(lightValues);

                        // Decrease chance of spawning so lampposts wont spawn really close to one another //
                        lamppostUnlikelyhood = lamppostChanceMultiplier;

                        if (lampPosts.size() == MAX_LIGHTS) return lampPosts;
                        
                    }

                    // Increase spawn likelyhood over time //
                    if (lamppostUnlikelyhood >= 1) lamppostUnlikelyhood -= 0.1f;
                }

                
            }
        }
    }

    return lampPosts;
}

XMFLOAT3 WorldGenerator::ChooseIntersection(XMFLOAT3 prevChoice, std::vector<XMFLOAT3> currentRoute, bool row)
{
    // Calculate position in the 2D array //
    std::vector<XMFLOAT3> intersections;
    int xPos = round(prevChoice.x / _tileSpacing);
    int yPos = round(prevChoice.z / _tileSpacing);

    // Select correct axis to search along //
    if (row) 
    {
        // Search along road for a crossroad //
        for (int i = 0; i < _worldTiles.size(); i++) 
        {
            GameObjectValues road = _worldTiles[i][yPos];
            if (road.name == "Road Crossroad" && !IsEqual(road.transform.position,prevChoice)) 
            {
                // Add to a list to be randomly selected from //
                intersections.push_back(road.transform.position);
            }
        }
    }
    else 
    {
        // Search along road for a crossroad //
        for (int i = 0; i < _worldTiles.size(); i++)
        {
            GameObjectValues road = _worldTiles[xPos][i];
            if (road.name == "Road Crossroad" && !IsEqual(road.transform.position, prevChoice))
            {
                // Add to a list to be randomly selected from //
                intersections.push_back(road.transform.position);
            }
        }
    }

    if (intersections.size() > 0) 
    {
        if (currentRoute.size() > 4) 
        {
            // Begin to search for start position//
            for (auto position : intersections)
            {
                if (position.x == currentRoute[0].x || position.z == currentRoute[0].z) return position;
            }
        }
        // Randomly choose a junction //
        return intersections[rand() % intersections.size()];
    }
    else return XMFLOAT3();
}

bool WorldGenerator::IsEqual(XMFLOAT3 A, XMFLOAT3 B)
{
    return (A.x == B.x && A.y == B.y && A.z == B.z);
}

void WorldGenerator::SetTileSpacing(float spacing)
{
    _tileSpacing = spacing;
}

float WorldGenerator::GetTileSpacing()
{
    return _tileSpacing;
}
