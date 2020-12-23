#pragma once

#include "Behaviours.h"

class ModuleBehaviour : public Module
{
public:

	bool update() override;

	Behaviour *			addBehaviour(BehaviourType behaviourType, GameObject *parentGameObject);
	Spaceship *			addSpaceship(GameObject *parentGameObject);
	Laser *				addLaser(GameObject *parentGameObject);
	AsteroidWeak *		addAsteroidWeak(GameObject* parentGameObject);
	AsteroidStrong *	addAsteroidStrong(GameObject* parentGameObject);

private:

	void handleBehaviourLifeCycle(Behaviour * behaviour);

	Spaceship spaceships[MAX_CLIENTS];
	Laser lasers[MAX_GAME_OBJECTS];
	AsteroidWeak asteroids_weak[MAX_GAME_OBJECTS];
	AsteroidStrong asteroids_strong[MAX_GAME_OBJECTS];
};

