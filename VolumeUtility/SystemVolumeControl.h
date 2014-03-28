#pragma once
class CSystemVolumeControl
{
    CComPtr<IAudioEndpointVolume> pAudioEndpointVolume;
public:
    CSystemVolumeControl(void);
    ~CSystemVolumeControl(void);

    HRESULT SetSysVolume(float fVolume);

private:
    HRESULT Initialize();
};

