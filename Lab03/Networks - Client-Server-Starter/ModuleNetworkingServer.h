#pragma once

#include "ModuleNetworking.h"

#define KICK_COMMAND_OFFSET 6
#define WHISPER_COMMAND_OFFSET 9
#define CHANGE_NAME_COMMAND_OFFSET 13

class ModuleNetworkingServer : public ModuleNetworking
{
public:

	//////////////////////////////////////////////////////////////////////
	// ModuleNetworkingServer public methods
	//////////////////////////////////////////////////////////////////////

	bool start(int port);

	bool isRunning() const;



private:

	//////////////////////////////////////////////////////////////////////
	// Module virtual methods
	//////////////////////////////////////////////////////////////////////

	bool update() override;

	bool gui() override;



	//////////////////////////////////////////////////////////////////////
	// ModuleNetworking virtual methods
	//////////////////////////////////////////////////////////////////////

	bool isListenSocket(SOCKET socket) const override;

	void onSocketConnected(SOCKET socket, const sockaddr_in &socketAddress) override;

	void onSocketReceivedData(SOCKET socket, const InputMemoryStream& packet) override;

	void onSocketDisconnected(SOCKET socket) override;



	//////////////////////////////////////////////////////////////////////
	// State
	//////////////////////////////////////////////////////////////////////

	enum class ServerState
	{
		Stopped,
		Listening
	};

	ServerState state = ServerState::Stopped;

	SOCKET listenSocket;

	struct ConnectedSocket
	{
		sockaddr_in address;
		SOCKET socket;
		std::string playerName;
		Color playerColor;
	};

	std::vector<ConnectedSocket> connectedSockets;
};

