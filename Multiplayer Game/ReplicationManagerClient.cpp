#include "Networks.h"
#include "ReplicationManagerClient.h"

// TODO(you): World state replication lab session

void ReplicationManagerClient::create(const InputMemoryStream& packet, const uint32& networkId) const
{

}

void ReplicationManagerClient::update(const InputMemoryStream& packet, const uint32& networkId) const
{

}

void ReplicationManagerClient::destroy(const InputMemoryStream& packet, const uint32& networkId) const
{

}

void ReplicationManagerClient::read(const InputMemoryStream& packet) const
{
	while (packet.RemainingByteCount() > 0)
	{

	}
}
