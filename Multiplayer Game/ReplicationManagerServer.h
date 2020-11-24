#pragma once

#include "ReplicationCommand.h"

// TODO(you): World state replication lab session
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