#pragma once
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>

class AudioPlayer
{
public:
	AudioPlayer(DWORD samplesPerSec, WORD bitsPerSample, WORD channels, WORD format, WORD blockAlign, DWORD avgBytesPerSec);
	~AudioPlayer();

	void writeAudioBlock(LPSTR block, DWORD size);

private:
	HWAVEOUT m_hWaveOut;

};

