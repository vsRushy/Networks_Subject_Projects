#pragma once

#include "ModuleNetworking.h"

#include <map>

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
	// Commands
	//////////////////////////////////////////////////////////////////////

	bool Help(SOCKET& socket, std::string p1, std::string p2, std::string message);
	bool Kick(SOCKET& socket, std::string p1, std::string p2, std::string message);
	bool List(SOCKET& socket, std::string p1, std::string p2, std::string message);
	bool Whisper(SOCKET& socket, std::string p1, std::string p2, std::string message);
	bool ChangeName(SOCKET& socket, std::string p1, std::string p2, std::string message);

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
	};

	std::vector<ConnectedSocket> connectedSockets;
	std::map<std::string, bool (ModuleNetworkingServer::*) (SOCKET& socket, std::string, std::string, std::string)> commandMap;
};

