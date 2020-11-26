#include "Networks.h"
#include "ReplicationManagerServer.h"

// TODO(you): World state replication lab session
void ReplicationManagerServer::create(const uint32& networkId)
{
	replicationCommands.insert_or_assign(networkId, ReplicationCommand(ReplicationAction::Create, networkId));
}

void ReplicationManagerServer::update(const uint32& networkId)
{
	replicationCommands.insert_or_assign(networkId, ReplicationCommand(ReplicationAction::Update, networkId));
}

void ReplicationManagerServer::destroy(const uint32& networkId)
{
	replicationCommands.insert_or_assign(networkId, ReplicationCommand(ReplicationAction::Destroy, networkId));
}

void ReplicationManagerServer::write(OutputMemoryStream& packet) const
{
	packet << PROTOCOL_ID;

	for (const auto& replicationCommand : replicationCommands)
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
				gameObject->networkInterpolationEnabled << gameObject->state;

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
	}
}

void ReplicationManagerServer::remove(const uint32& networkId)
{
	replicationCommands.erase(networkId);
}
