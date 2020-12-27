#include "Networks.h"
#include "ModuleBehaviour.h"

static float map(float x, float in_min, float in_max, float out_min, float out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void ModuleBehaviour::AsteroidRainData::Generate()
{
	currentTime = 0.f;
	float randomUnit = Random.next();
	nextSpawnTime = map(randomUnit, 0.0f, 1.0f, spawnTimeRange.first, spawnTimeRange.second);
}

bool ModuleBehaviour::init()
{
	asteroidData.Generate();

	return true;
}

bool ModuleBehaviour::update()
{
	for (Spaceship &behaviour : spaceships)
	{
		handleBehaviourLifeCycle(&behaviour);
	}
	
	for (Laser &behaviour : lasers)
	{
		handleBehaviourLifeCycle(&behaviour);
	}

	for (Asteroid& behaviour : asteroids)
	{
		handleBehaviourLifeCycle(&behaviour);
	}


	return true;
}

void ModuleBehaviour::ServerUpdate() // called from networking server update
{
	/*if (startGame == 0)
		return;*/
	AsteroidRain();
}

void ModuleBehaviour::AsteroidRain()
{

	if ((asteroidData.currentTime += Time.deltaTime) >= asteroidData.nextSpawnTime)
	{
		asteroidData.Generate();
		SpawnAsteroid();
	}
}


void ModuleBehaviour::SpawnAsteroid()
{
	// Create a new game object with the player properties
	GameObject* gameObject = NetworkInstantiate();
	gameObject->position = vec2{ 2000.f * (Random.next() - 0.5f), -500.0f};
	gameObject->angle = 0;

	// Decide weak or strong
	bool weak = false;
	if (Random.next() >= 0.5f)
		weak = true;

	// Create behaviour
	Asteroid* asteroidBehaviour = (Asteroid*)App->modBehaviour->addBehaviour(BehaviourType::Asteroid, gameObject);
	gameObject->behaviour = dynamic_cast<Behaviour*>(asteroidBehaviour);
	gameObject->behaviour->isServer = true;

	// Random size between a min and a max value
	float randomUnit = Random.next();
	float sideSize = map(randomUnit, 0.0f, 1.0f, (float)asteroidBehaviour->sizeRange.first, (float)asteroidBehaviour->sizeRange.second);
	gameObject->size = { sideSize, sideSize };

	// Damage according to size
	asteroidBehaviour->hitPoints = (uint8)(int)map(randomUnit, 0.0f, 1.0f, (float)(int)asteroidBehaviour->hitPointRange.first, (float)(int)asteroidBehaviour->hitPointRange.second);

	// Speed according to size (inverse)
	float newSpeed = (uint8)(int)map(randomUnit, 0.0f, 1.0f, (float)(int)asteroidBehaviour->speedRange.second, (float)(int)asteroidBehaviour->speedRange.first);
	asteroidBehaviour->speed = { 0.0f, newSpeed };

	/*if (weak == false)
		asteroidBehaviour->hitPoints = asteroidBehaviour->MAX_HIT_POINTS;*/

	// Create sprite
	gameObject->sprite = App->modRender->addSprite(gameObject);
	gameObject->sprite->order = 6;


	if (weak) {
		gameObject->sprite->texture = App->modResources->asteroid1;
	}
	else {
		gameObject->sprite->texture = App->modResources->asteroid2;
	}


	// Create collider
	gameObject->collider = App->modCollision->addCollider(ColliderType::Asteroid, gameObject);

}

Behaviour *ModuleBehaviour::addBehaviour(BehaviourType behaviourType, GameObject *parentGameObject)
{
	switch (behaviourType)
	{
	case BehaviourType::Spaceship:
		return addSpaceship(parentGameObject);
	case BehaviourType::Laser:
		return addLaser(parentGameObject);
	case BehaviourType::Asteroid:
		return addAsteroid(parentGameObject);
	default:
		return nullptr;
	}
}

Spaceship *ModuleBehaviour::addSpaceship(GameObject *parentGameObject)
{

	// for testing purposes
	startGame++;

	for (Spaceship &behaviour : spaceships)
	{
		if (behaviour.gameObject == nullptr)
		{
			behaviour = {};
			behaviour.gameObject = parentGameObject;
			parentGameObject->behaviour = &behaviour;
			return &behaviour;
		}
	}

	ASSERT(false);
	return nullptr;
}

Laser *ModuleBehaviour::addLaser(GameObject *parentGameObject)
{
	for (Laser &behaviour : lasers)
	{
		if (behaviour.gameObject == nullptr)
		{
			behaviour = {};
			behaviour.gameObject = parentGameObject;
			parentGameObject->behaviour = &behaviour;
			return &behaviour;
		}
	}

	ASSERT(false);
	return nullptr;
}

Asteroid* ModuleBehaviour::addAsteroid(GameObject* parentGameObject)
{
	for (Asteroid& behaviour : asteroids)
	{
		if (behaviour.gameObject == nullptr)
		{
			behaviour = {};
			behaviour.gameObject = parentGameObject;
			parentGameObject->behaviour = &behaviour;
			return &behaviour;
		}
	}

	ASSERT(false);
	return nullptr;
}


void ModuleBehaviour::handleBehaviourLifeCycle(Behaviour *behaviour)
{
	GameObject *gameObject = behaviour->gameObject;

	if (gameObject != nullptr)
	{
		switch (gameObject->state)
		{
		case GameObject::STARTING:
			behaviour->start();
			break;
		case GameObject::UPDATING:
			behaviour->update();
			break;
		case GameObject::DESTROYING:
			behaviour->destroy();
			gameObject->behaviour = nullptr;
			behaviour->gameObject = nullptr;
			break;
		default:;
		}
	}
}


