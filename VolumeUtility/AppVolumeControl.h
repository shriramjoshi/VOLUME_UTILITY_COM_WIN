#pragma once

class CAppVolumeControl
{
    float m_fVolume;
    int m_iSessionCount;
    int m_iActiveSessions;
    CComPtr<IAudioSessionEnumerator> pAudioSessionEnumerator;

public:
    CAppVolumeControl(void);
    ~CAppVolumeControl(void);
    HRESULT SetAppVolume(std::string strAppName, float fVolumeChange);

private:
    HRESULT SetVolume(CComPtr<ISimpleAudioVolume> pSimpleAudioVolume, float fVolume);
    float GetVolume(void) const;
    int GetAudioSessionCount(void);

    HRESULT Initialize(void);
    BOOL IsSessionActive(CComPtr<IAudioSessionControl>pAudioSessionControl);
    BOOL CheckProcess(DWORD pid, std::string strAppName);
};

