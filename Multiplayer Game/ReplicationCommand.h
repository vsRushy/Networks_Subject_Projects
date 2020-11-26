#pragma once

// TODO(you): World state replication lab session
enum class ReplicationAction
{
	None,
	Create,
	Update,
	Destroy
};

struct ReplicationCommand
{
	ReplicationCommand()
		: action(ReplicationAction::None), networkId(NULL)
	{

	}

	ReplicationCommand(const ReplicationAction& action, const uint32& networkId)
		: action(action), networkId(networkId)
	{

	}

	ReplicationAction action;
	uint32 networkId;
};