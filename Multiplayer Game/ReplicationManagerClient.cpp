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
				gameObject->angle;

			bool has_sprite = false;
			packet >> has_sprite;
			if (has_sprite)
			{
				Sprite* sprite = gameObject->sprite;
				sprite = App->modRender->addSprite(gameObject);
				std::string texture_name;
				packet >> texture_name;
				sprite->texture = App->modResources->GetTexture(texture_name);
				//packet >> sprite->order;
			}

			break;
		}
		case ReplicationAction::Update:
		{
			gameObject = App->modLinkingContext->getNetworkGameObject(replicationCommand.networkId);

			packet >> gameObject->position.x >> gameObject->position.y >>
				gameObject->size.x >> gameObject->size.y >>
				gameObject->angle;

			bool has_sprite = false;
			packet >> has_sprite;
			if (has_sprite)
			{
				Sprite* sprite = gameObject->sprite;
				std::string texture_name;
				packet >> texture_name;
				//packet >> sprite->order;
			}

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
