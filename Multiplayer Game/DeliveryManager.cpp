#include "Networks.h"
#include "DeliveryManager.h"

// TODO(you): Reliability on top of UDP lab session

DeliveryDelegate::Delivery* DeliveryManager::writeSequenceNumber(OutputMemoryStream fcpacket)
{
	// create delegate (will handle success/failure)
	DeliveryDelegate* delegate = new DeliveryDelegate; 

	// create delivery
	DeliveryDelegate::Delivery* d = new DeliveryDelegate::Delivery;

	// assign delegate to delivery
	d->delegate = delegate;

	// assign sequence number and increase it
	d->sequenceHumber = nextOutgoingSequenceNumber++;

	// write it in the packet
	fcpacket << d->sequenceHumber;

	// assign time
	d->dispatchTime = Time.time;

	// push it to the list
	pendingDeliveries.push_back(d);

	return d;
}

bool DeliveryManager::processSequenceNumber(const InputMemoryStream& packet)
{
	// capture the packet's sequence number
	uint32 packetSequenceNumber = UINT32_MAX;
	packet >> packetSequenceNumber;

	// If it fits with the expected number...
	if (packetSequenceNumber >= nextExpectedSequenceNumber)
	{
		// ...  push the number to pending acks
		sequenceNumbersPendingAck.push_back(packetSequenceNumber);

		// deserialize

		return true;
	}
		

	return false;
}

void DeliveryDelegate::onDeliverySuccess(DeliveryManager* deliveryManager)
{

}

void DeliveryDelegate::onDeliveryFailure(DeliveryManager* deliveryManager)
{

}
