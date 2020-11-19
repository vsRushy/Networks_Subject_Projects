#pragma once

// TODO(you): World state replication lab session
class ReplicationManagerClient
{
public:
	void create(const InputMemoryStream& packet, const uint32& networkId) const;
	void update(const InputMemoryStream& packet, const uint32& networkId) const;
	void destroy(const InputMemoryStream& packet, const uint32& networkId) const;

	void read(const InputMemoryStream& packet) const;
};