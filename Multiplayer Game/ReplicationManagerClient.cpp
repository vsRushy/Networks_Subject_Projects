#include "Networks.h"
#include "ReplicationManagerClient.h"
#include "ReplicationCommand.h"

// TODO(you): World state replication lab session
void ReplicationManagerClient::read(const InputMemoryStream& packet) const
{
	while (packet.RemainingByteCount() > 0)
	{
		uint32 protocol_id;
		packet >> protocol_id;

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
				gameObject->networkInterpolationEnabled >> gameObject->state;

			break;
		}
		case ReplicationAction::Update:
		{
			gameObject = App->modLinkingContext->getNetworkGameObject(replicationCommand.networkId);

			packet >> gameObject->position.x >> gameObject->position.y >>
				gameObject->size.x >> gameObject->size.y >>
				gameObject->angle >> gameObject->tag >>
				gameObject->networkInterpolationEnabled >> gameObject->state;

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
