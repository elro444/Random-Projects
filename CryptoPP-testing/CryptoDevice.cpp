#include "CryptoDevice.h"




CryptoDevice::CryptoDevice(std::string sKey, std::string sIv)
{
	if (CryptoPP::AES::DEFAULT_KEYLENGTH < sKey.size())
		sKey = sKey.substr(0, CryptoPP::AES::DEFAULT_KEYLENGTH);
	else if (CryptoPP::AES::DEFAULT_KEYLENGTH > sKey.size())
		sKey += std::string(CryptoPP::AES::DEFAULT_KEYLENGTH - sKey.size(), ' ');


	if (CryptoPP::AES::BLOCKSIZE < sIv.size())
		sIv = sIv.substr(0, CryptoPP::AES::BLOCKSIZE);
	else if (CryptoPP::AES::BLOCKSIZE > sIv.size())
		sIv += std::string(CryptoPP::AES::BLOCKSIZE - sIv.size(), '*');


	memcpy(iv, sIv.c_str(), CryptoPP::AES::BLOCKSIZE);
	memcpy(key, sKey.c_str(), CryptoPP::AES::DEFAULT_KEYLENGTH);

}

CryptoDevice::~CryptoDevice() {}


std::wstring CryptoDevice::encryptAES(std::wstring plainText)
{

	wchar_t* cipherText = new wchar_t[plainText.length() + 1];
	for (unsigned i = 0; i < plainText.length(); i++)
		cipherText[i] = L'\0';
	//getchar();
	//cout << "inEncrypt:\n" << plainText << endl;
	CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption cfbEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH, iv);
	cfbEncryption.ProcessData((byte*)cipherText, (byte*)plainText.c_str(), plainText.length() * 2 + 1);

	std::wstring ans(cipherText, plainText.size());
	//getchar();
	//cout << ans << endl;
	delete[] cipherText;
	return ans;
}

std::wstring CryptoDevice::decryptAES(std::wstring cipherText)
{

	wchar_t *decryptedText = new wchar_t[cipherText.length() + 1];
	for (unsigned i = 0; i < cipherText.length(); i++)
		decryptedText[i] = L'\0';



	CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption cfbDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH, iv);
	cfbDecryption.ProcessData((byte*)decryptedText, (byte*)cipherText.c_str(), cipherText.length() * 2 + 1);

	std::wstring ans(decryptedText, cipherText.size());
	delete[] decryptedText;
	//cout << ans << endl;
	return ans;
}

std::string CryptoDevice::hash(const std::string& input)
{
	std::string output;
	CryptoPP::SHA512 sha512;
	CryptoPP::HashFilter filter(sha512, new CryptoPP::HexEncoder(new CryptoPP::StringSink(output)));
	CryptoPP::ChannelSwitch cs;
	cs.AddDefaultRoute(filter);
	CryptoPP::StringSource ss(input, true, new CryptoPP::Redirector(cs));
	return output;
}

std::string CryptoDevice::hash(const std::wstring& input)
{
	std::string cInput;

	int req = WideCharToMultiByte(CP_UTF8, 0, input.c_str(), (int)input.length(), NULL, 0, NULL, NULL);
	if (req < 0 || req == 0)
		throw std::runtime_error("Failed to convert string");

	cInput.resize((size_t)req);

	int cch = WideCharToMultiByte(CP_UTF8, 0, input.c_str(), (int)input.length(), &cInput[0], (int)cInput.length(), NULL, NULL);
	if (cch < 0 || cch == 0)
		throw std::runtime_error("Failed to convert string");

	// Should not be required
	cInput.resize((size_t)cch);

	return hash(cInput);
}
