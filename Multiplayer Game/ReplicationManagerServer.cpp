#include "Networks.h"
#include "ReplicationManagerServer.h"

// TODO(you): World state replication lab session
void ReplicationManagerServer::create(const uint32& networkId)
{
	std::unordered_map<uint32, ReplicationCommand>::const_iterator item = replicationCommands.find(networkId);
	if (item != replicationCommands.end())
		return; // Already exists
	else
		replicationCommands[networkId] = ReplicationCommand(ReplicationAction::Create, networkId);
}

void ReplicationManagerServer::update(const uint32& networkId)
{
	std::unordered_map<uint32, ReplicationCommand>::const_iterator item = replicationCommands.find(networkId);
	if (item != replicationCommands.end())
		return; // Already exists
	else if (replicationCommands[networkId].action != ReplicationAction::Create &&
		replicationCommands[networkId].action != ReplicationAction::Destroy)
		replicationCommands[networkId] = ReplicationCommand(ReplicationAction::Update, networkId);
}

void ReplicationManagerServer::destroy(const uint32& networkId)
{
	std::unordered_map<uint32, ReplicationCommand>::const_iterator item = replicationCommands.find(networkId);
	if (item != replicationCommands.end())
		return; // Already exists
	else
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
				gameObject->angle;

			// Sprite
			if (gameObject->sprite != nullptr)
			{
				packet << true << std::string(gameObject->sprite->texture->filename) << gameObject->sprite->order;
			}
			else
			{
				packet << false;
			}

			// Collider
			if (gameObject->collider != nullptr)
			{
				packet << true << gameObject->collider->type << gameObject->collider->isTrigger;
			}
			else
			{
				packet << false;
			}

			// Behaviour
			// TODO

			break;
		}

		case ReplicationAction::Update:
		{
			packet << gameObject->position.x << gameObject->position.y <<
				gameObject->size.x << gameObject->size.y <<
				gameObject->angle;

			// Behaviour ?

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
