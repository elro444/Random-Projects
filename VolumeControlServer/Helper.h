#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <nlohmann/json.hpp>
#include <string>

using nlohmann::json;
enum class MessageCode;

class Helper
{
public:
	static void getMessage(SOCKET socket, MessageCode& code, json& params);
	static void sendMessage(SOCKET socket, MessageCode code, json params);
	static json::value_type getWithDefault(json theJson, std::string key, json::value_type defaultValue = NULL);

private:
	static std::string getPartFromSocket(SOCKET socket, int len);
};

enum class MessageCode
{
	Unknown = 0,

	// Right after connecting
	Greet = 1, 
	GreetRes,

	// When it's client's turn:
	GetVolume,
	GetVolumeRes,
	SetVolume,
	SetVolumeRes,
	GetMute,
	GetMuteRes,
	SetMute,
	SetMuteRes,
	ToggleMute,
	ToggleMuteRes,

	// When Pending:
	P_GetRemaining = 64,
	P_GetRemainingRes,
	P_KillAllOthers,
	P_KillAllOthersRes,

	// Global:
	Disconnet = 100
	//DisconnectRes? nah
};