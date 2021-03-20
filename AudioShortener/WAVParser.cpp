#include "WAVParser.h"

WAVParser::WAVParser(std::ifstream& infile) :
	riffHeader{ 0 },
	fmtChunk{ 0 },
	dataChunk{ 0 }
{
	parse(infile);
}

WAVParser::WAVParser(const std::string& inPath) :
	WAVParser(std::ifstream(inPath, std::ios::binary))
{
}

WAVParser::WAVParser(const std::wstring& inPath) :
	WAVParser(std::ifstream(inPath, std::ios::binary))
{
}

void WAVParser::parse(std::ifstream& infile)
{
	while (infile.good())
	{
		char groupID[4] = { 0 };
		infile.read(groupID, 4);
		if (infile.eof())
			break;
		if (!memcmp(groupID, "RIFF", 4))
			parseHeader(infile);
		else if (!memcmp(groupID, "fmt ", 4))
			parseFormat(infile);
		else if (!memcmp(groupID, "data", 4))
			parseData(infile);
		else
			skipChunk(infile);

		if (!infile) // EOF while reading etc..
			throw ParseError();
	}
}

void WAVParser::parseHeader(std::ifstream& infile)
{
	infile.read(reinterpret_cast<char*>(&riffHeader.size), 4);
	infile.read(riffHeader.riffType, 4);
	if (memcmp(riffHeader.riffType, "WAVE", 4))
		throw NotAWavFile();
}

void WAVParser::parseFormat(std::ifstream& infile)
{
	infile.read(reinterpret_cast<char*>(&fmtChunk.chunkSize), 4);
	infile.read(reinterpret_cast<char*>(&fmtChunk.formatTag), 2);
	infile.read(reinterpret_cast<char*>(&fmtChunk.channels), 2);
	infile.read(reinterpret_cast<char*>(&fmtChunk.samplesPerSec), 4);
	infile.read(reinterpret_cast<char*>(&fmtChunk.avgBytesPerSec), 4);
	infile.read(reinterpret_cast<char*>(&fmtChunk.blockAlign), 2);
	infile.read(reinterpret_cast<char*>(&fmtChunk.bitsPerSample), 2);
}

void WAVParser::parseData(std::ifstream& infile)
{
	infile.read(reinterpret_cast<char*>(&dataChunk.chunkSize), 4);
	dataChunk.data.resize(dataChunk.chunkSize);
	infile.read(dataChunk.data.data(), dataChunk.data.size());
}

void WAVParser::skipChunk(std::ifstream& infile)
{
	sf::Uint32 size = 0;
	infile.read(reinterpret_cast<char*>(&size), 4);
	infile.seekg(size, std::ios::cur);
}
