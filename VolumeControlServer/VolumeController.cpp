#include "VolumeController.h"
#include <exception>
#define THROW_ON_ERROR(hr) if (FAILED(hr)) {throw std::exception("Failed initializing at" __FUNCTION__);}


VolumeController::VolumeController() :
	m_listener(NULL),
	m_endPtVol(NULL)
{
	HRESULT hr = S_OK;
	hr = CoInitialize(NULL);


	hr = CoCreateGuid(&m_guid);
	THROW_ON_ERROR(hr);

	// Get enumerator for audio endpoint devices.
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
		NULL, CLSCTX_INPROC_SERVER,
		__uuidof(IMMDeviceEnumerator),
		(void**)&m_enumerator);
	THROW_ON_ERROR(hr);

	// Get default audio-rendering device.
	hr = m_enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &m_device);
	THROW_ON_ERROR(hr);

	hr = m_device->Activate(__uuidof(IAudioEndpointVolume),
		CLSCTX_ALL, NULL, (void**)&m_endPtVol);
	THROW_ON_ERROR(hr);
}


VolumeController::~VolumeController()
{
	SAFE_RELEASE(m_enumerator);
	SAFE_RELEASE(m_device);
	SAFE_RELEASE(m_endPtVol);
	CoUninitialize();
}

int VolumeController::getMasterVolume() const
{
	float fVolume = 0;
	m_endPtVol->GetMasterVolumeLevelScalar(&fVolume); // Scalar means percentage. 'non-scalar' is in dB..
	return static_cast<int>(fVolume * 100.f);
}

bool VolumeController::getMute() const
{
	BOOL bMute;
	m_endPtVol->GetMute(&bMute);
	return static_cast<bool>(bMute);
}

void VolumeController::setMasterVolume(int newVolume)
{
	m_endPtVol->SetMasterVolumeLevelScalar(newVolume / 100.f, &m_guid);
}

void VolumeController::setMute(bool newMute)
{
	m_endPtVol->SetMute(static_cast<BOOL>(newMute), &m_guid);
}

void VolumeController::toggleMute()
{
	setMute(!getMute());
}

void VolumeController::setListener(BaseOnVolumeChangeListener * listener)
{
	clearListener();

	if (listener != NULL)
	{
		m_listener = listener;
		m_endPtVol->RegisterControlChangeNotify(m_listener);
		m_listener->setContext(m_guid);
	}
}

void VolumeController::clearListener()
{
	if (m_enumerator != NULL && m_listener != NULL)
	{
		m_endPtVol->UnregisterControlChangeNotify(m_listener);
		m_listener->setContext(GUID_NULL);
	}
	m_listener = NULL;
}
