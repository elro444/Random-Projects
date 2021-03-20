#pragma once
#include <Windows.h>
#include "VolumeNotifier.h"

class VolumeController
{
public:
	VolumeController();
	~VolumeController();

	int getMasterVolume() const;
	bool getMute() const;

	void setMasterVolume(int newVolume);
	void setMute(bool newMute);
	void toggleMute();

	void setListener(BaseOnVolumeChangeListener *listener);
	void clearListener();

private:
	GUID m_guid;
	IAudioEndpointVolume *m_endPtVol;
	IMMDevice *m_device;
	BaseOnVolumeChangeListener *m_listener;
	IMMDeviceEnumerator *m_enumerator;
};

