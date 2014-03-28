// VolumeUtility.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "AppVolumeControl.h"
#include "SystemVolumeControl.h"
#include<Windows.h>
#include<stdlib.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <psapi.h>

int _tmain(int argc, _TCHAR* argv[])
{
    HRESULT hr = S_OK;
    std::wstring wstrAppName(argv[1]);
    std::string strApp(wstrAppName.begin(), wstrAppName.end());
    float fVolume = _tstof(argv[2]);
    if(fVolume >= 0.0f && fVolume <= 1.0f)
    {
        if((strApp.find("sys") != -1) || (strApp.find("Sys") != -1)) {
            CSystemVolumeControl systemVolumeControl;
            systemVolumeControl.SetSysVolume(fVolume);
        }
        else
        {
            CAppVolumeControl appVolumeControl;
            std::wstring wstrAppName(argv[1]);
            std::string strApp(wstrAppName.begin(), wstrAppName.end());
            appVolumeControl.SetAppVolume(strApp, fVolume);
        }
    }
    else printf("\n Volume change value is out of range.\n Please give input volume in range 0.0 to 1.0");
    return 0;
}
