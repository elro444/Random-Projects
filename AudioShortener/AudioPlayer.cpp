#include "AudioPlayer.h"
#include <exception>


AudioPlayer::AudioPlayer(DWORD samplesPerSec, WORD bitsPerSample, WORD channels, WORD format, WORD blockAlign, DWORD avgBytesPerSec) :
	m_hWaveOut(0)
{
	WAVEFORMATEX wfx; /* look this up in your documentation */
	MMRESULT result;/* for waveOut return values */
	/*
	 * first we need to set up the WAVEFORMATEX structure.
	 * the structure describes the format of the audio.
	 */
	wfx.nSamplesPerSec = samplesPerSec; /* sample rate */
	wfx.wBitsPerSample = bitsPerSample; /* sample size */
	wfx.nChannels = channels; /* channels*/
	/*
	 * WAVEFORMATEX also has other fields which need filling.
	 * as long as the three fields above are filled this should
	 * work for any PCM (pulse code modulation) format.
	 */
	wfx.cbSize = 0; /* size of _extra_ info */
	wfx.wFormatTag = format;
	wfx.nBlockAlign = blockAlign;
	wfx.nAvgBytesPerSec = avgBytesPerSec;
	/*
	 * try to open the default wave device. WAVE_MAPPER is
	 * a constant defined in mmsystem.h, it always points to the
	 * default wave device on the system (some people have 2 or
	 * more sound cards).
	 */
	if (waveOutOpen(
		&m_hWaveOut,
		WAVE_MAPPER,
		&wfx,
		0,
		0,
		CALLBACK_NULL) != MMSYSERR_NOERROR)
	{
		fprintf(stderr, "Error while opening");
		throw std::exception("unable to open WAVE_MAPPER device");
	}
}

AudioPlayer::~AudioPlayer()
{
	waveOutClose(m_hWaveOut);
}

void AudioPlayer::writeAudioBlock(LPSTR block, DWORD size)
{
	WAVEHDR header;
	/*
	 * initialise the block header with the size
	 * and pointer.
	 */
	ZeroMemory(&header, sizeof(WAVEHDR));
	header.dwBufferLength = size;
	header.lpData = block;
	/*
	 * prepare the block for playback
	 */
	waveOutPrepareHeader(m_hWaveOut, &header, sizeof(WAVEHDR));
	/*
	 * write the block to the device. waveOutWrite returns immediately
	 * unless a synchronous driver is used (not often).
	 */
	waveOutWrite(m_hWaveOut, &header, sizeof(WAVEHDR));
	/*
	 * wait a while for the block to play then start trying
	 * to unprepare the header. this will fail until the block has
	 * played.
	 */
	Sleep(500);
	while (waveOutUnprepareHeader(
		m_hWaveOut,
		&header,
		sizeof(WAVEHDR)
	) == WAVERR_STILLPLAYING)
		Sleep(100);
}