#include "Networks.h"
#include "ReplicationManagerServer.h"

// TODO(you): World state replication lab session
ReplicationCommand::ReplicationCommand(const ReplicationAction& action, const uint32& networkId)
	: action(action), networkId(networkId)
{

}

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
	for (const auto& replicationCommand : replicationCommands)
	{
		uint32 networkId = replicationCommand.second.networkId;
		packet.Write(networkId);

		ReplicationAction replicationAction = replicationCommand.second.action;
		packet.Write(replicationAction);
		
		switch (replicationAction)
		{
		case ReplicationAction::Create:
		case ReplicationAction::Update:
		{
			GameObject* gameObject = App->modLinkingContext->getNetworkGameObject(networkId);

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
