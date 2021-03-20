#include "Helper.h"

void Helper::getMessage(SOCKET sock, MessageCode& code, json& params)
{
	//Protocol:
	//[code][len-4][json_bytes]
	std::string codeBuffer = getPartFromSocket(sock, 1);
	if (!codeBuffer.length() || codeBuffer[0] == '\0') // client disconnected
	{
		code = MessageCode::Unknown;
		params = json();
		return;
	}
	code = static_cast<MessageCode>(codeBuffer[0]);
	std::string lengthBuffer = getPartFromSocket(sock, 4);
	int length = 0;
	for (unsigned i = 0; i < lengthBuffer.length(); i++)
		length += static_cast<int>(lengthBuffer[i]) * pow(256, i);

	std::string jsonString = getPartFromSocket(sock, length);
	params = json::parse(jsonString);
	printf("Recieved from %d: [%d][%d][%s]\n", sock, code, length, jsonString.c_str());
}

void Helper::sendMessage(SOCKET sock, MessageCode code, json params)
{
	std::string jsonString = params.dump();
	int jsonLength = jsonString.length();
	std::string jsonLengthString(4, 0); // length 4, fill with 0's
	jsonLengthString[0] = jsonLength & 0xFF;
	jsonLengthString[1] = jsonLength & (0xFF << 8);
	jsonLengthString[2] = jsonLength & (0xFF << 16);
	jsonLengthString[3] = jsonLength & (0xFF << 24);
	std::string message;
	message.push_back(static_cast<char>(code));
	message += jsonLengthString;
	message += jsonString;
	printf("Sending from %d: [%d][%d][%s]\n", sock, code, jsonLength, jsonString.c_str());
	if (::send(sock, message.data(), message.size(), 0) < 0)
	{
		printf("Error while sending! LastError: %d\n", WSAGetLastError());
		//throw std::exception(("Error while sending to" + std::to_string(sock)).c_str());
	}
}

json::value_type Helper::getWithDefault(json theJson, std::string key, json::value_type defaultValue)
{
	json::const_iterator it = theJson.find(key);
	if (it == theJson.end())
		return defaultValue;
	else
		return it->get<decltype(defaultValue)>();
}


std::string Helper::getPartFromSocket(SOCKET socket, int len)
{
	std::string output(len + 1, 0);
	if (::recv(socket, &output[0], len, 0) < 0)
	{
		printf("Error while getting part! LastError: %d\n", WSAGetLastError());
		//throw std::exception(("Error while reading from" + std::to_string(socket)).c_str());
	}
	return output;
}