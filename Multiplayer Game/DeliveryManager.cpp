#include "Networks.h"
#include "DeliveryManager.h"

// TODO(you): Reliability on top of UDP lab session

DeliveryDelegate::Delivery* DeliveryManager::writeSequenceNumber(OutputMemoryStream fcpacket)
{
	// create delegate (will handle success/failure)
	DeliveryDelegate* delegate = new DeliveryDelegate;  // TODO(us): leaks? 

	// create delivery
	DeliveryDelegate::Delivery* d = new DeliveryDelegate::Delivery;

	// assign delegate to delivery
	d->delegate = delegate;

	// assign sequence number and increase it
	d->sequenceNumber = nextOutgoingSequenceNumber++;

	// write it in the packet
	fcpacket << d->sequenceNumber;

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
		return true;
	}
		

	return false;
}

bool DeliveryManager::hasSequenceNumbersPendingAck() const
{
	return false;
}


void DeliveryManager::writeSequenceNumbersPendingAck(OutputMemoryStream Apacket)
{
	
}


/*
Process the acknowledged seq. numbers processAckdSequenceNumbers()
	- For each sequence number, find the packet delivery information
	- Invoke its callbacks onSuccess()/onFailure() as needed
	- Remove the delivery
*/

void DeliveryManager::processAckdSequenceNumbers(const InputMemoryStream& packet) // what are we supposed to do with the packet?? 
{
	/*uint32 protoId;
	packet >> protoId;
	if (protoId != PROTOCOL_ID) return;

	ServerMessage message;
	packet >> message;

	uint32 packetSequenceNumber = UINT32_MAX;
	packet >> packetSequenceNumber;
	*/
	

	for (auto pNumber = sequenceNumbersPendingAck.begin(); pNumber != sequenceNumbersPendingAck.end(); ++pNumber)
	{

		for (auto pDelivery = pendingDeliveries.begin(); pDelivery != pendingDeliveries.end(); 	++pDelivery)
		{
			if ((*pDelivery)->sequenceNumber == (*pNumber))
			{
				(*pDelivery)->delegate->onDeliverySuccess(this);
			}
			else
			{
				(*pDelivery)->delegate->onDeliveryFailure(this);
			}
			
			pDelivery = pendingDeliveries.erase(pDelivery);

			// TODO: delete delivery and delegate

		}
	}
}

void DeliveryManager::processTimedOutPackets()
{
}

void DeliveryManager::clear()
{

}

void DeliveryDelegate::onDeliverySuccess(DeliveryManager* deliveryManager)
{
	OutputMemoryStream fcpacket;
	deliveryManager->writeSequenceNumber(fcpacket);
}

void DeliveryDelegate::onDeliveryFailure(DeliveryManager* deliveryManager)
{

}
