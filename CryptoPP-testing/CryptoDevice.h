#pragma once

#include <windows.h>
#include <cstdio>
#include <iostream>
#include <CryptoPP\osrng.h>
#include <CryptoPP\modes.h>
#include <string.h>
#include <cstdlib>
#include <CryptoPP\hex.h>
#include <CryptoPP\channels.h>



class CryptoDevice
{

public:
	CryptoDevice(std::string, std::string);
	~CryptoDevice();
	std::wstring encryptAES(std::wstring);
	std::wstring decryptAES(std::wstring);
	static std::string hash(const std::string&);
	static std::string hash(const std::wstring&);
	byte key[CryptoPP::AES::DEFAULT_KEYLENGTH], iv[CryptoPP::AES::BLOCKSIZE];
};
