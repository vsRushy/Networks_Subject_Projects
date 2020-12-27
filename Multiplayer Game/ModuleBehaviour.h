#pragma once

#include "Behaviours.h"

class ModuleBehaviour : public Module
{
public:

	bool init() override;
	bool update() override;
	void ServerUpdate(); // create asteroirds and stuff here

	Behaviour *			addBehaviour(BehaviourType behaviourType, GameObject *parentGameObject);
	Spaceship *			addSpaceship(GameObject *parentGameObject);
	Laser *				addLaser(GameObject *parentGameObject);
	Asteroid *		addAsteroid(GameObject* parentGameObject);


	// Gameplay functions 
	void AsteroidRain();
	void SpawnAsteroid();

private:

	// for test purpooses
	uint8 startGame = 0;

	struct AsteroidRainData
	{
		float currentTime = 0.f;
		float nextSpawnTime = 0.f;
		std::pair<float, float> spawnTimeRange = std::make_pair(0.5f, 1.8f);
		void Generate();
	} asteroidData;

	void handleBehaviourLifeCycle(Behaviour * behaviour);

	Spaceship spaceships[MAX_CLIENTS];
	Laser lasers[MAX_GAME_OBJECTS];
	Asteroid asteroids[MAX_GAME_OBJECTS];
};

