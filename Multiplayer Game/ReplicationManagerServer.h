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
	ReplicationCommand();
	ReplicationCommand(const ReplicationAction& action, const uint32& networkId);

	ReplicationAction action;
	uint32 networkId;
};

class ReplicationManagerServer
{
public:
	void create(const uint32& networkId);
	void update(const uint32& networkId);
	void destroy(const uint32& networkId);

	void write(OutputMemoryStream& packet) const;

	void remove(const uint32& networkId);

private:
	std::unordered_map<uint32, ReplicationCommand> replicationCommands;
};