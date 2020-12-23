#include "Networks.h"
#include "ReplicationManagerClient.h"
#include "ReplicationCommand.h"

// TODO(you): World state replication lab session
void ReplicationManagerClient::read(const InputMemoryStream& packet) const
{
	while (packet.RemainingByteCount() > 0)
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
			App->modLinkingContext->registerNetworkGameObjectWithNetworkId(gameObject, replicationCommand.networkId);

			packet >> gameObject->position.x >> gameObject->position.y >>
				gameObject->size.x >> gameObject->size.y >>
				gameObject->angle >> gameObject->tag >>
				gameObject->networkInterpolationEnabled;

			bool has_sprite = false;
			packet >> has_sprite;
			if (has_sprite)
			{
				gameObject->sprite = App->modRender->addSprite(gameObject);
				std::string texture_name;
				packet >> texture_name;
				gameObject->sprite->texture = App->modResources->GetTexture(texture_name);
				packet >> gameObject->sprite->order;
			}

			bool has_collider = false;
			packet >> has_collider;
			if (has_collider)
			{
				ColliderType collider = ColliderType::None;
				packet >> collider;
				gameObject->collider = App->modCollision->addCollider(collider, gameObject);
				packet >> gameObject->collider->isTrigger;
			}

			bool has_behaviour = false;
			packet >> has_behaviour;
			if (has_behaviour)
			{
				BehaviourType b_type = BehaviourType::None;
				packet >> b_type;

				gameObject->behaviour = App->modBehaviour->addBehaviour(b_type, gameObject);
				gameObject->behaviour->read(packet);
			}

			break;
		}
		case ReplicationAction::Update:
		{
			gameObject = App->modLinkingContext->getNetworkGameObject(replicationCommand.networkId);

			if (gameObject != nullptr)
			{
				packet >> gameObject->position.x >> gameObject->position.y >>
					gameObject->size.x >> gameObject->size.y >>
					gameObject->angle >> gameObject->tag >>
					gameObject->networkInterpolationEnabled;

				bool has_behaviour = false;
				packet >> has_behaviour;
				if (has_behaviour)
				{
					gameObject->behaviour->read(packet);
				}
			}

			break;
		}
		case ReplicationAction::Destroy:
		{
			gameObject = App->modLinkingContext->getNetworkGameObject(replicationCommand.networkId);
			if (gameObject != nullptr)
			{
				App->modLinkingContext->unregisterNetworkGameObject(gameObject);
				App->modGameObject->Destroy(gameObject);
			}

			break;
		}
		default:
		{
			break;
		}
		}
	}
}
