#include "Networks.h"
#include "ReplicationManagerServer.h"

// TODO(you): World state replication lab session
void ReplicationManagerServer::create(const uint32& networkId)
{
	replicationCommands[networkId] = ReplicationCommand(ReplicationAction::Create, networkId);
}

void ReplicationManagerServer::update(const uint32& networkId)
{
	if (replicationCommands[networkId].action != ReplicationAction::Create || replicationCommands[networkId].action != ReplicationAction::Destroy)
		replicationCommands[networkId].action = ReplicationAction::Update;
}

void ReplicationManagerServer::destroy(const uint32& networkId)
{
	replicationCommands[networkId].action = ReplicationAction::Destroy;
}

void ReplicationManagerServer::write(OutputMemoryStream& packet)
{
	packet << PROTOCOL_ID;
	packet << ServerMessage::Replication;

	for (auto& replicationCommand : replicationCommands)
	{
		uint32 networkId = replicationCommand.first;
		packet << networkId;

		ReplicationAction replicationAction = replicationCommand.second.action;
		packet << replicationAction;

		GameObject* gameObject = App->modLinkingContext->getNetworkGameObject(networkId);
		
		switch (replicationAction)
		{
		case ReplicationAction::Create:
		{
			packet << gameObject->position.x << gameObject->position.y <<
				gameObject->size.x << gameObject->size.y <<
				gameObject->angle << gameObject->tag <<
				gameObject->networkInterpolationEnabled;

			if (gameObject->sprite != nullptr)
			{
				packet << true << std::string(gameObject->sprite->texture->filename);
				packet << gameObject->sprite->order;
			}
			else
			{
				packet << false;
			}

			if (gameObject->collider != nullptr)
			{
				packet << true << gameObject->collider->type << gameObject->collider->isTrigger;
			}
			else
			{
				packet << false;
			}

			if (gameObject->behaviour != nullptr)
			{
				packet << true << gameObject->behaviour->type();
				gameObject->behaviour->write(packet);
			}
			else
			{
				packet << false;
			}

			break;
		}
		case ReplicationAction::Update:
		{
			packet << gameObject->position.x << gameObject->position.y <<
				gameObject->size.x << gameObject->size.y <<
				gameObject->angle << gameObject->tag <<
				gameObject->networkInterpolationEnabled;

			if (gameObject->behaviour != nullptr)
			{
				packet << true;
				gameObject->behaviour->write(packet);
			}
			else
			{
				packet << false;
			}

			break;
		}

		case ReplicationAction::Destroy:
		{
			// TODO
			
			break;
		}

		default:
		{
			break;
		}
		}

		replicationCommand.second.action = ReplicationAction::None;
	}
}

void ReplicationManagerServer::remove(const uint32& networkId)
{
	replicationCommands.erase(networkId);
}
