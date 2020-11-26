#pragma once

// TODO(you): Reliability on top of UDP lab session

class DeliveryManager
{
public:
	// For senders to write a new seq. numbers into a packet Delivery * writeSequenceNu*ber(OutputMemoryStream fcpacket);
	// For receivers to process the seq. number from an incoming packet bool processSequenceNumber(const InpufeiroryStream &packet);
	// For receivers to write ack'ed seq. numbers into a packet bool hasSequenceNumbersPendingAck() const;
	void writeSequenceNumbersPendingAck(OutputMemoryStream Apacket);
	// For senders to process ack'ed seq. numbers from a packet void processAckdSequenceNumbers(const InputttemoryStream &packet); void processTimedOutPackets();
	void clear();

private:
	// Pr�vate members (sender side)
	// - The next outgoing sequence number // - A list of pending deliveries
	// Pr�vate members (receiver side)
	// - The next expected sequence number // - A list of sequence numbers pending ack

};

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