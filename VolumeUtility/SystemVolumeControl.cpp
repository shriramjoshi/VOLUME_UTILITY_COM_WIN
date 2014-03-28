#include "StdAfx.h"
#include "SystemVolumeControl.h"


CSystemVolumeControl::CSystemVolumeControl(void)
{
    CoInitialize(NULL);
}


CSystemVolumeControl::~CSystemVolumeControl(void)
{
    CoUninitialize();
}

HRESULT CSystemVolumeControl::Initialize()
{
      HRESULT hr = S_OK;
      CComPtr<IMMDeviceEnumerator> deviceEnumerator = NULL;
      hr = deviceEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
      if(FAILED(hr))
      {
          //error handling
          return hr;
      }
      
      CComPtr<IMMDevice> defaultDevice = NULL;
      hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
      if(FAILED(hr))
      {
          //error handling
          return hr;
      }
      hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, 
          reinterpret_cast<void **> (&this->pAudioEndpointVolume));
      if(FAILED(hr))
      {
          //error handling
          return hr;
      }
      return hr;
}

HRESULT CSystemVolumeControl::SetSysVolume(float fVolume)
{
      HRESULT hr = S_OK;
      hr = this->Initialize();
      if(FAILED(S_OK))
      {
          //error handling
          return hr;
      }
      float currentVolume = 0;
      hr = this->pAudioEndpointVolume->GetMasterVolumeLevelScalar(&currentVolume);
      printf("Current volume as a scalar is: %f\n", currentVolume);
      
      PROCESS_INFORMATION processInformation = { 0 };
      STARTUPINFO startupInfo = { 0 };
      std::string strTemp = "C:\\Windows\\System32\\SndVol.exe";
      std::string strTempCmd = " -f 39274520";
      std::wstring strVolMixer = std::wstring(strTemp.begin(), strTemp.end());
      BSTR strCmdParam = _com_util::ConvertStringToBSTR(strTempCmd.c_str());
      LPCWSTR lpWStrVolMixer = strVolMixer.c_str();
      LPWSTR lpCmdParam = strCmdParam;
      BOOL result = CreateProcessW(lpWStrVolMixer, lpCmdParam, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &startupInfo, &processInformation);
      Sleep(1000);
      hr = pAudioEndpointVolume->SetMasterVolumeLevelScalar(fVolume, NULL);

      hr = this->pAudioEndpointVolume->GetMasterVolumeLevelScalar(&currentVolume);
      printf("Current volume as a scalar is: %f\n", currentVolume);
      //endpointVolume->Release();
      
      return hr;
}
