// Epvolume.h -- Implementation of IAudioEndpointVolumeCallback interface
#pragma once
#define IDC_SLIDER_VOLUME      1001
#define IDC_CHECK_MUTE         1002
#define IDC_STATIC_MINVOL      1003
#define IDC_STATIC_MAXVOL      1004
#pragma comment(lib, "Comctl32.lib")

#include <windows.h>
#include <commctrl.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <stdio.h>

#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

//-----------------------------------------------------------
// Client implementation of IAudioEndpointVolumeCallback
// interface. When a method in the IAudioEndpointVolume
// interface changes the volume level or muting state of the
// endpoint device, the change initiates a call to the
// client's IAudioEndpointVolumeCallback::OnNotify method.
//-----------------------------------------------------------
class BaseOnVolumeChangeListener : public IAudioEndpointVolumeCallback
{
	LONG _cRef;

	//See OnNotify()
	virtual HRESULT STDMETHODCALLTYPE onVolumeNotification(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify) = 0;

protected:
	GUID m_context;
public:
	BaseOnVolumeChangeListener() :
		_cRef(1),
		m_context(GUID_NULL)
	{
	}

	~BaseOnVolumeChangeListener()
	{
	}

	void setContext(GUID guid)
	{
		m_context = guid;
	}

	// IUnknown methods -- AddRef, Release, and QueryInterface

	ULONG STDMETHODCALLTYPE AddRef()
	{
		return InterlockedIncrement(&_cRef);
	}

	ULONG STDMETHODCALLTYPE Release()
	{
		ULONG ulRef = InterlockedDecrement(&_cRef);
		if (0 == ulRef)
		{
			delete this;
		}
		return ulRef;

	}

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID **ppvInterface)
	{
		if (IID_IUnknown == riid)
		{
			AddRef();
			*ppvInterface = (IUnknown*)this;
		}
		else if (__uuidof(IAudioEndpointVolumeCallback) == riid)
		{
			AddRef();
			*ppvInterface = (IAudioEndpointVolumeCallback*)this;
		}
		else
		{
			*ppvInterface = NULL;
			return E_NOINTERFACE;
		}
		return S_OK;
	}

	// Callback method for endpoint-volume-change notifications.

	HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify)
	{
		// I just wanted a prettier name ^_^
		return onVolumeNotification(pNotify);
	}
};