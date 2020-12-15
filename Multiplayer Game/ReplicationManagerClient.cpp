#include "Networks.h"
#include "ReplicationManagerClient.h"
#include "ReplicationCommand.h"

// TODO(you): World state replication lab session
void ReplicationManagerClient::read(const InputMemoryStream& packet) const
{
	while ((int)packet.RemainingByteCount() > 0)
	{
		ReplicationCommand replicationCommand;

		packet >> replicationCommand.networkId;
		packet >> replicationCommand.action;

		GameObject* gameObject = nullptr;

		switch (replicationCommand.action)
		{
		case ReplicationAction::Create:
		{
			gameObject = App->modGameObject->Instantiate();

			if (App->modLinkingContext->getNetworkGameObject(replicationCommand.networkId) != nullptr)
			{
				packet >> gameObject->position.x >> gameObject->position.y >>
					gameObject->size.x >> gameObject->size.y >>
					gameObject->angle;

				// Sprite
				bool has_sprite = false;
				packet >> has_sprite;
				if (has_sprite)
				{
					gameObject->sprite = App->modRender->addSprite(gameObject);
					std::string sprite_file;
					packet >> sprite_file;
					gameObject->sprite->texture = App->modResources->GetTextureByPath(sprite_file);
					packet >> gameObject->sprite->order;
				}

				// Collider
				bool has_collider = false;
				packet >> has_collider;
				if (has_collider)
				{
					ColliderType collider_type = ColliderType::None;
					packet >> collider_type;

					gameObject->collider = App->modCollision->addCollider(collider_type, gameObject);
					packet >> gameObject->collider->isTrigger;
				}

				// Behaviour
				/*bool has_behaviour = false;
				packet >> has_behaviour;
				if (has_behaviour)
				{
					BehaviourType behaviour_type = BehaviourType::None;
					packet >> behaviour_type;

					gameObject->behaviour = App->modBehaviour->addBehaviour(behaviour_type, gameObject);
					// TODO
				}*/

				App->modGameObject->Destroy(gameObject);
			}
			else
			{
				App->modLinkingContext->registerNetworkGameObjectWithNetworkId(gameObject, replicationCommand.networkId);

				packet >> gameObject->position.x >> gameObject->position.y >>
					gameObject->size.x >> gameObject->size.y >>
					gameObject->angle;

				// Sprite
				bool has_sprite = false;
				packet >> has_sprite;
				if (has_sprite)
				{
					gameObject->sprite = App->modRender->addSprite(gameObject);
					std::string sprite_file;
					packet >> sprite_file;
					gameObject->sprite->texture = App->modResources->GetTextureByPath(sprite_file);
					packet >> gameObject->sprite->order;
				}

				// Collider
				bool has_collider = false;
				packet >> has_collider;
				if (has_collider)
				{
					ColliderType collider_type = ColliderType::None;
					packet >> collider_type;

					gameObject->collider = App->modCollision->addCollider(collider_type, gameObject);
					packet >> gameObject->collider->isTrigger;
				}

				// Behaviour
				/*bool has_behaviour = false;
				packet >> has_behaviour;
				if (has_behaviour)
				{
					BehaviourType behaviour_type = BehaviourType::None;
					packet >> behaviour_type;

					gameObject->behaviour = App->modBehaviour->addBehaviour(behaviour_type, gameObject);
					// TODO
				}*/

				/*if (replicationCommand.networkId == App->modNetClient->getNetworkId())
				{
					gameObject->networkInterpolationEnabled = false;
				}*/
			}

			break;
		}
		case ReplicationAction::Update:
		{
			gameObject = App->modLinkingContext->getNetworkGameObject(replicationCommand.networkId);

			packet >> gameObject->position.x >> gameObject->position.y >>
				gameObject->size.x >> gameObject->size.y >>
				gameObject->angle;

			// Behaviour?

			break;
		}
		case ReplicationAction::Destroy:
		{
			gameObject = App->modLinkingContext->getNetworkGameObject(replicationCommand.networkId);
			App->modLinkingContext->unregisterNetworkGameObject(gameObject);

			App->modGameObject->Destroy(gameObject);

			break;
		}
		default:
		{
			break;
		}
		}
	}
}
