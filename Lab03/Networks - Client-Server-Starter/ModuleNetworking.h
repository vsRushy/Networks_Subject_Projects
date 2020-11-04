#pragma once

struct Color
{
	Color()
	{
		r = 1.0f; g = 1.0f, b = 1.0f, a = 1.0f;
	}

	Color(const float& r, const float& g, const float& b, const float& a) : r(r), g(g), b(b), a(a)
	{

	}

	float r, g, b, a;
};

class ModuleNetworking : public Module
{
private:

	//////////////////////////////////////////////////////////////////////
	// Module virtual methods
	//////////////////////////////////////////////////////////////////////

	bool init() override;

	bool preUpdate() override;

	bool cleanUp() override;



	//////////////////////////////////////////////////////////////////////
	// Socket event callbacks
	//////////////////////////////////////////////////////////////////////

	virtual bool isListenSocket(SOCKET socket) const { return false; }

	virtual void onSocketConnected(SOCKET socket, const sockaddr_in &socketAddress) { }

	virtual void onSocketReceivedData(SOCKET s, const InputMemoryStream& packet) = 0;

	virtual void onSocketDisconnected(SOCKET s) = 0;

	

protected:

	std::vector<SOCKET> sockets;

	void addSocket(SOCKET socket);

	void disconnect();

	static void reportError(const char *message);

	static bool sendPacket(const OutputMemoryStream& packet, SOCKET socket);
};

