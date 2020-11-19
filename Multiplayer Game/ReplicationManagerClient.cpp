#include "Networks.h"
#include "ReplicationManagerClient.h"

// TODO(you): World state replication lab session
void ReplicationManagerClient::read(const InputMemoryStream& packet) const
{
	while (packet.RemainingByteCount() > 0)
	{
		ReplicationCommand replicationCommand;

		packet >> replicationCommand.networkId;
		packet >> replicationCommand.action;

		GameObject* gameObject = App->modLinkingContext->getNetworkGameObject(replicationCommand.networkId);

		switch (replicationCommand.action)
		{
		case ReplicationAction::Create:
		{


			break;
		}
		case ReplicationAction::Update:
		{


			break;
		}
		case ReplicationAction::Destroy:
		{
			

			break;
		}
		default:
		{
			break;
		}
		}
	}
}
