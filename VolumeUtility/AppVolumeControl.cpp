#include "StdAfx.h"
#include "stdafx.h"
#include "AppVolumeControl.h"
#include <psapi.h>

CAppVolumeControl::CAppVolumeControl(void)
{
    this->m_fVolume = 0.0f;
    this->m_iSessionCount = 0;
    this->m_iActiveSessions = 0;
    CoInitialize(NULL);
}


CAppVolumeControl::~CAppVolumeControl(void)
{
    CoUninitialize();
}

HRESULT CAppVolumeControl::Initialize()
{
    HRESULT hr = S_OK;

    CComPtr<IMMDeviceEnumerator> pMMDeviceEnumerator;
    hr = pMMDeviceEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
    if(FAILED(hr)) {
        //error handling
    }

    CComPtr<IMMDevice> pMMDevice;
    hr = pMMDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pMMDevice);
    if(FAILED(hr)) {
        //error handling
    }

    CComPtr<IAudioSessionManager2> pAudioSessionManager;
    hr = pMMDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, nullptr, reinterpret_cast<void **>(&pAudioSessionManager));
    if(FAILED(hr)) {
        //error handling
    }

    //CComPtr<IAudioSessionEnumerator> pAudioSessionEnumerator;
    hr = pAudioSessionManager->GetSessionEnumerator(&this->pAudioSessionEnumerator);
    if(FAILED(hr)) {
        //error handling
    }
    
    return hr;
}

BOOL CAppVolumeControl::IsSessionActive(CComPtr<IAudioSessionControl> pAudioSessionControl)
{
    AudioSessionState audioSessionState;
    HRESULT hr = pAudioSessionControl->GetState(&audioSessionState);
    if(FAILED(hr))
    {
        //error handling
        return FALSE;
    }
    if(audioSessionState != AudioSessionStateActive)
    {
        return FALSE;
    }
    this->m_iActiveSessions++;
    return TRUE;
}

HRESULT CAppVolumeControl::SetVolume(CComPtr<ISimpleAudioVolume> pSimpleAudioVolume, float fVolume)
{
    HRESULT hr = S_OK;
    //ISimpleAudioVolume *pTempSimpleAudioVolume = *pSimpleAudioVolume;
    hr = pSimpleAudioVolume->SetMasterVolume(fVolume, NULL);
    if(FAILED(hr))
    {
        //error handling
    }
    return hr;
}

int CAppVolumeControl::GetAudioSessionCount(void)
{
    this->pAudioSessionEnumerator->GetCount(&this->m_iSessionCount);
    return this->m_iSessionCount;
}

BOOL CAppVolumeControl::CheckProcess(DWORD pid, std::string strAppName)
{
    HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
    HMODULE hMod;
    DWORD cbNeeded;
    if ( EnumProcessModules(handle, &hMod, sizeof(hMod), &cbNeeded))
    {
        GetModuleBaseNameW(handle, hMod, szProcessName, sizeof(szProcessName)/sizeof(TCHAR));
    }
    std::wstring wstr(szProcessName);
    std::string strProcessName(wstr.begin(), wstr.end());
    if(strProcessName.find(strAppName) != -1) 
    {
        return TRUE;
    }
    return FALSE;
}

HRESULT CAppVolumeControl::SetAppVolume(std::string strAppName, float fVolumeChange)
{
    HRESULT hr = S_OK;
    hr = this->Initialize();
    if(FAILED(hr)) 
    {
        //error handling
    }
    int iSessionCount = this->GetAudioSessionCount();
    float fCurrentVolume = 0.0f;
    for(int i = 0; i < iSessionCount; i++)
    {
        CComPtr<IAudioSessionControl> pAudioSessionControl;
        hr = this->pAudioSessionEnumerator->GetSession(i, &pAudioSessionControl);
        if(FAILED(hr))
        {
            //error handling
            return hr;
        }
        if(this->IsSessionActive(pAudioSessionControl))
        {
            this->m_iActiveSessions++;
            CComPtr<ISimpleAudioVolume> pSimpleAudioVolume;
            hr = pAudioSessionControl->QueryInterface(__uuidof(ISimpleAudioVolume),
                reinterpret_cast<void **>(&pSimpleAudioVolume));
            pSimpleAudioVolume->GetMasterVolume(&fCurrentVolume);
            if(FAILED(hr))
            {
                //error handling
                return hr;
            }
            if(strAppName == "all")
            {
                hr = this->SetVolume(pSimpleAudioVolume, fVolumeChange);
                if(FAILED(hr))
                {
                    //error handling
                    return hr;
                }
            }
            else
            {
                DWORD pid = 0;
                CComPtr<IAudioSessionControl2> pAudioSessionControl2;
                hr = pAudioSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), reinterpret_cast<void **>(&pAudioSessionControl2));
                if (FAILED(hr))
                {
                    //error handling
                    return hr;
                }
                hr = pAudioSessionControl2->GetProcessId(&pid);
                if(this->CheckProcess(pid, strAppName))
                {
                    hr = this->SetVolume(pSimpleAudioVolume, fVolumeChange);
                    if(FAILED(hr))
                    {
                        //error handling
                        return hr;
                    }
                }
            }
        }
    }
    return hr;
}
