#pragma once
#include <fstream>
#include <exception>
#include <vector>
#include <SFML/Config.hpp>

class WAVParser
{
public:
	WAVParser(std::ifstream& infile);
	WAVParser(const std::string& inPath);
	WAVParser(const std::wstring& inPath);

	class ParseError : public std::exception {};
	class NotAWavFile : public ParseError {};

	struct
	{
		//char groupID[4];
		sf::Uint32 size;
		char riffType[4];
	} riffHeader;

	struct
	{
		//char groupID[4];
		sf::Uint32 chunkSize; // Size of rest of the chunk
		sf::Uint16 formatTag;
		sf::Uint16 channels;
		sf::Uint32 samplesPerSec;
		sf::Uint32 avgBytesPerSec;
		sf::Uint16 blockAlign;
		sf::Uint16 bitsPerSample;
	} fmtChunk;

	struct
	{
		//char groupID[4];
		sf::Int32 chunkSize;
		std::vector<char> data;
	} dataChunk;
private:

	void parse(std::ifstream& infile);
	void parseHeader(std::ifstream& infile);
	void parseFormat(std::ifstream& infile);
	void parseData(std::ifstream& infile);
	void skipChunk(std::ifstream& infile);
};

