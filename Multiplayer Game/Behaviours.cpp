#include "Networks.h"
#include "Behaviours.h"



void Laser::start()
{
	gameObject->networkInterpolationEnabled = false;

	App->modSound->playAudioClip(App->modResources->audioClipLaser);
}

void Laser::update()
{
	secondsSinceCreation += Time.deltaTime;

	const float pixelsPerSecond = 1000.0f;
	gameObject->position += vec2FromDegrees(gameObject->angle) * pixelsPerSecond * Time.deltaTime;

	if (isServer)
	{
		const float neutralTimeSeconds = 0.1f;
		if (secondsSinceCreation > neutralTimeSeconds && gameObject->collider == nullptr) {
			gameObject->collider = App->modCollision->addCollider(ColliderType::Laser, gameObject);
		}

		const float lifetimeSeconds = 2.0f;
		if (secondsSinceCreation >= lifetimeSeconds) {
			NetworkDestroy(gameObject);
		}
	}
}



void Spaceship::start()
{
	gameObject->tag = (uint32)(Random.next() * UINT_MAX);

	lifebar = Instantiate();
	lifebar->sprite = App->modRender->addSprite(lifebar);
	lifebar->sprite->pivot = vec2{ 0.0f, 0.5f };
	lifebar->sprite->order = 5;
}

void Spaceship::onInput(const InputController &input)
{
	if (input.horizontalAxis != 0.0f)
	{
		const float rotateSpeed = 180.0f;
		gameObject->angle += input.horizontalAxis * rotateSpeed * Time.deltaTime;

		if (isServer)
		{
			NetworkUpdate(gameObject);
		}
	}

	if (input.actionDown == ButtonState::Pressed)
	{
		const float advanceSpeed = 200.0f;
		gameObject->position += vec2FromDegrees(gameObject->angle) * advanceSpeed * Time.deltaTime;

		if (isServer)
		{
			NetworkUpdate(gameObject);
		}
	}

	if (input.actionLeft == ButtonState::Press)
	{
		if (isServer)
		{
			switch (weapon)
			{
			case WEAPON::FRONT:
			{
				GameObject* laser = NetworkInstantiate();

				laser->position = gameObject->position;
				laser->angle = gameObject->angle;
				laser->size = { 20, 60 };

				laser->sprite = App->modRender->addSprite(laser);
				laser->sprite->order = 3;
				laser->sprite->texture = App->modResources->laser;

				Laser* laserBehaviour = App->modBehaviour->addLaser(laser);
				laserBehaviour->isServer = isServer;

				laser->tag = gameObject->tag;

				break;
			}
			case WEAPON::AXIS:
			{
				GameObject* laser1 = NetworkInstantiate();

				laser1->position = gameObject->position;
				laser1->angle = gameObject->angle;
				laser1->size = { 20, 60 };
				laser1->sprite = App->modRender->addSprite(laser1);
				laser1->sprite->order = 3;
				laser1->sprite->texture = App->modResources->laser;
				Laser* laserBehaviour1 = App->modBehaviour->addLaser(laser1);
				laserBehaviour1->isServer = isServer;
				laser1->tag = gameObject->tag;

				GameObject* laser2 = NetworkInstantiate();
				laser2->position = gameObject->position;
				laser2->angle = gameObject->angle + 90.0f;
				laser2->size = { 20, 60 };
				laser2->sprite = App->modRender->addSprite(laser2);
				laser2->sprite->order = 3;
				laser2->sprite->texture = App->modResources->laser;
				Laser* laserBehaviour2 = App->modBehaviour->addLaser(laser2);
				laserBehaviour2->isServer = isServer;
				laser2->tag = gameObject->tag;

				GameObject* laser3 = NetworkInstantiate();
				laser3->position = gameObject->position;
				laser3->angle = gameObject->angle + 180.0f;
				laser3->size = { 20, 60 };
				laser3->sprite = App->modRender->addSprite(laser3);
				laser3->sprite->order = 3;
				laser3->sprite->texture = App->modResources->laser;
				Laser* laserBehaviour3 = App->modBehaviour->addLaser(laser3);
				laserBehaviour3->isServer = isServer;
				laser3->tag = gameObject->tag;

				GameObject* laser4 = NetworkInstantiate();
				laser4->position = gameObject->position;
				laser4->angle = gameObject->angle + 270.0f;
				laser4->size = { 20, 60 };
				laser4->sprite = App->modRender->addSprite(laser4);
				laser4->sprite->order = 3;
				laser4->sprite->texture = App->modResources->laser;
				Laser* laserBehaviour4 = App->modBehaviour->addLaser(laser4);
				laserBehaviour4->isServer = isServer;
				laser4->tag = gameObject->tag;

				break;
			}
			case WEAPON::DIAGONALS:
			{
				GameObject* laser1 = NetworkInstantiate();

				laser1->position = gameObject->position;
				laser1->angle = gameObject->angle;
				laser1->size = { 20, 60 };
				laser1->sprite = App->modRender->addSprite(laser1);
				laser1->sprite->order = 3;
				laser1->sprite->texture = App->modResources->laser;
				Laser* laserBehaviour1 = App->modBehaviour->addLaser(laser1);
				laserBehaviour1->isServer = isServer;
				laser1->tag = gameObject->tag;

				GameObject* laser2 = NetworkInstantiate();
				laser2->position = gameObject->position;
				laser2->angle = gameObject->angle;
				laser2->size = { 20, 60 };
				laser2->sprite = App->modRender->addSprite(laser2);
				laser2->sprite->order = 3;
				laser2->sprite->texture = App->modResources->laser;
				Laser* laserBehaviour2 = App->modBehaviour->addLaser(laser2);
				laserBehaviour2->isServer = isServer;
				laser2->tag = gameObject->tag;

				GameObject* laser3 = NetworkInstantiate();
				laser3->position = gameObject->position;
				laser3->angle = gameObject->angle;
				laser3->size = { 20, 60 };
				laser3->sprite = App->modRender->addSprite(laser3);
				laser3->sprite->order = 3;
				laser3->sprite->texture = App->modResources->laser;
				Laser* laserBehaviour3 = App->modBehaviour->addLaser(laser3);
				laserBehaviour3->isServer = isServer;
				laser3->tag = gameObject->tag;

				GameObject* laser4 = NetworkInstantiate();
				laser4->position = gameObject->position;
				laser4->angle = gameObject->angle;
				laser4->size = { 20, 60 };
				laser4->sprite = App->modRender->addSprite(laser4);
				laser4->sprite->order = 3;
				laser4->sprite->texture = App->modResources->laser;
				Laser* laserBehaviour4 = App->modBehaviour->addLaser(laser4);
				laserBehaviour4->isServer = isServer;
				laser4->tag = gameObject->tag;

				break;
			}
			default:
			{
				break;
			}
			}
		}
	}

	if (input.actionUp == ButtonState::Release)
	{
		weapon < num_weapons ? ++weapon : weapon = WEAPON::FRONT;

		if (isServer)
		{
			NetworkUpdate(gameObject);
		}
	}
}

void Spaceship::update()
{
	static const vec4 colorAlive = vec4{ 0.2f, 1.0f, 0.1f, 0.5f };
	static const vec4 colorDead = vec4{ 1.0f, 0.2f, 0.1f, 0.5f };
	const float lifeRatio = max(0.01f, (float)(hitPoints) / (MAX_HIT_POINTS));
	lifebar->position = gameObject->position + vec2{ -50.0f, -50.0f };
	lifebar->size = vec2{ lifeRatio * 80.0f, 5.0f };
	lifebar->sprite->color = lerp(colorDead, colorAlive, lifeRatio);
}

void Spaceship::destroy()
{
	Destroy(lifebar);
}

void Spaceship::onCollisionTriggered(Collider &c1, Collider &c2)
{
	if (c2.type == ColliderType::Laser && c2.gameObject->tag != gameObject->tag)
	{
		if (isServer)
		{
			NetworkDestroy(c2.gameObject); // Destroy the laser
		
			if (hitPoints > 0)
			{
				hitPoints--;
				NetworkUpdate(gameObject);
			}

			float size = 30 + 50.0f * Random.next();
			vec2 position = gameObject->position + 50.0f * vec2{Random.next() - 0.5f, Random.next() - 0.5f};

			if (hitPoints <= 0)
			{
				// Centered big explosion
				size = 250.0f + 100.0f * Random.next();
				position = gameObject->position;

				NetworkDestroy(gameObject);
			}

			GameObject *explosion = NetworkInstantiate();
			explosion->position = position;
			explosion->size = vec2{ size, size };
			explosion->angle = 365.0f * Random.next();

			explosion->sprite = App->modRender->addSprite(explosion);
			explosion->sprite->texture = App->modResources->explosion1;
			explosion->sprite->order = 100;

			explosion->animation = App->modRender->addAnimation(explosion);
			explosion->animation->clip = App->modResources->explosionClip;

			// TODO (custom) (replication)
			explosion->animation->animation_type = Animation::AnimationType::Explosion;

			NetworkDestroy(explosion, 2.0f);

			// NOTE(jesus): Only played in the server right now...
			// You need to somehow make this happen in clients
			App->modSound->playAudioClip(App->modResources->audioClipExplosion);
		}
	}
}

void Spaceship::write(OutputMemoryStream & packet)
{
	packet << hitPoints;
}

void Spaceship::read(const InputMemoryStream & packet)
{
	packet >> hitPoints;
}



void AsteroidStrong::start()
{

}

void AsteroidStrong::update()
{

}

void AsteroidStrong::destroy()
{

}

void AsteroidStrong::onCollisionTriggered(Collider& c1, Collider& c2)
{

}

void AsteroidStrong::write(OutputMemoryStream& packet)
{

}

void AsteroidStrong::read(const InputMemoryStream& packet)
{

}



void AsteroidWeak::start()
{

}

void AsteroidWeak::update()
{

}

void AsteroidWeak::destroy()
{

}

void AsteroidWeak::onCollisionTriggered(Collider& c1, Collider& c2)
{

}

void AsteroidWeak::write(OutputMemoryStream& packet)
{

}

void AsteroidWeak::read(const InputMemoryStream& packet)
{

}
