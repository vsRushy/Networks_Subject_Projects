#pragma once

// Add as many messages as you need depending on the
// functionalities that you decide to implement.

enum class ClientMessage
{
	Hello,
	Chat
};

enum class ServerMessage
{
	Welcome,
	NonWelcome,
	Chat,
	Help,
	List,
	Disconnect,
	Whisper,
	ChangeClientName,
	InvalidCommand,
	Clear,
	ChangeClientColor,
	RockPaperScissors,
	AlreadyUsedName,
	ClientConnected,
	ClientDisconnected
};

