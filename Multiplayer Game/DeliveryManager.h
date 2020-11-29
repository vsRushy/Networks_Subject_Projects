#pragma once

// TODO(you): Reliability on top of UDP lab session
class DeliveryManager;
class DeliveryDelegate
{
public:
	virtual void onDeliverySuccess(DeliveryManager* deliveryManager);
	virtual void onDeliveryFailure(DeliveryManager* deliveryManager);
	struct Delivery
	{
		uint32 sequenceHumber = 0;
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
	bool processSequenceNumber(const InputMemoryStream &packet);
	// For receivers to write ack'ed seq. numbers into a packet 
	bool hasSequenceNumbersPendingAck() const;
	void writeSequenceNumbersPendingAck(OutputMemoryStream Apacket);
	// For senders to process ack'ed seq. numbers from a packet 
	void processAckdSequenceNumbers(const InputMemoryStream &packet);
	void processTimedOutPackets();
	void clear();

private:
	// Prívate members (sender side)
	// - The next outgoing sequence number // - A list of pending deliveries
	// Prívate members (receiver side)
	// - The next expected sequence number // - A list of sequence numbers pending ack

};
