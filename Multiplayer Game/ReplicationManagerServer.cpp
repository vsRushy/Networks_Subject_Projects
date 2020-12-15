#include "Networks.h"
#include "ReplicationManagerServer.h"

// TODO(you): World state replication lab session
void ReplicationManagerServer::create(const uint32& networkId)
{
	replicationCommands[networkId] = ReplicationCommand(ReplicationAction::Create, networkId);
}

void ReplicationManagerServer::update(const uint32& networkId)
{
	if (replicationCommands[networkId].action == ReplicationAction::Create || replicationCommands[networkId].action == ReplicationAction::Destroy)
		return;

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
		case ReplicationAction::Update:
		{
			packet << gameObject->position.x << gameObject->position.y <<
				gameObject->size.x << gameObject->size.y <<
				gameObject->angle << gameObject->tag <<
				gameObject->networkInterpolationEnabled;

			if (gameObject->sprite != nullptr)
			{
				Sprite* sprite = gameObject->sprite;
				packet << true << std::string(sprite->texture->filename);
				//packet << sprite->order;
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
