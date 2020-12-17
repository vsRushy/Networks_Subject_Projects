#pragma once
#include <list>

// TODO(you): Reliability on top of UDP lab session
class DeliveryManager;
class DeliveryDelegate
{
public:
	virtual void onDeliverySuccess(DeliveryManager* deliveryManager);
	virtual void onDeliveryFailure(DeliveryManager* deliveryManager);
	struct Delivery
	{
		uint32 sequenceNumber = 0;
		double dispatchTime = 0.0;
		DeliveryDelegate* delegate = nullptr;
	};
};

class DeliveryManager
{
public:
	// For senders to write a new seq. numbers into a packet 
	DeliveryDelegate::Delivery * writeSequenceNumber(OutputMemoryStream fcpacket);
	// For receivers to process the seq. number from an incoming packet 
	bool processSequenceNumber(uint32 packetSequenceNumber = UINT32_MAX);
	// For receivers to write ack'ed seq. numbers into a packet 
	bool hasSequenceNumbersPendingAck() const;
	void writeSequenceNumbersPendingAck(OutputMemoryStream Apacket);
	// For senders to process ack'ed seq. numbers from a packet 
	void processAckdSequenceNumbers(const InputMemoryStream &packet);
	void processTimedOutPackets();
	void clear();

private:
	// Prívate members (sender side)
	// - The next outgoing sequence number
	uint32 nextOutgoingSequenceNumber = 0;

	// - A list of pending deliveries
	std::list<DeliveryDelegate::Delivery*> pendingDeliveries;

	// Prívate members (receiver side)
	// - The next expected sequence number 
	uint32 nextExpectedSequenceNumber = 0;

	// - A list of sequence numbers pending ack
	std::list<uint32> sequenceNumbersPendingAck;

};
