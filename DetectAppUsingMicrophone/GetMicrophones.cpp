#include "DetectAppUsingMicrophone.h"

#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }

std::vector<microphone> GetMicrophones() {
    std::vector<microphone> microphones(0);

    HRESULT hr = S_OK;
    IMMDeviceEnumerator* pEnumerator = NULL;
    IAudioSessionManager2* pSessionManager = NULL;
    BOOL result = FALSE;

    hr = CoInitialize(0);

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);

    IMMDeviceCollection* dCol = NULL;
    hr = pEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &dCol);
    UINT dCount;
    hr = dCol->GetCount(&dCount);
    for (UINT i = 0; i < dCount; i++) {
        IMMDevice* pCaptureDevice = NULL;
        hr = dCol->Item(i, &pCaptureDevice);

        IPropertyStore* pProps = NULL;
        hr = pCaptureDevice->OpenPropertyStore(STGM_READ, &pProps);

        PROPVARIANT varName;
        PropVariantInit(&varName);

        hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);

        std::wstring nameStr(varName.pwszVal);

        std::size_t found = nameStr.find(L"Microphone");
        if (found == std::string::npos) {
            continue;
        }

        // printf("Endpoint frinendly name: \"%S\"\n", varName.pwszVal);

        hr = pCaptureDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void**)&pSessionManager);

        if (!pSessionManager) {
            continue;
        }

        std::vector<session> new_sessions(0);
        microphone new_microphone = { varName.pwszVal, new_sessions };
        microphones.push_back(new_microphone);

        int cbSessionCount = 0;

        IAudioSessionEnumerator* pSessionList = NULL;
        IAudioSessionControl* pSessionControl = NULL;
        IAudioSessionControl2* pSessionControl2 = NULL;

        hr = pSessionManager->GetSessionEnumerator(&pSessionList);

        hr = pSessionList->GetCount(&cbSessionCount);
        // wprintf_s(L"Session count: %d\n", cbSessionCount);

        for (int index = 0; index < cbSessionCount; index++) {
            hr = pSessionList->GetSession(index, &pSessionControl);

            hr = pSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&pSessionControl2);

            hr = pSessionControl2->IsSystemSoundsSession();
            if (S_OK == hr) {
                continue;
            }

            LPWSTR instId = NULL;
            hr = pSessionControl2->GetSessionInstanceIdentifier(&instId);
            if (S_OK == hr) {
                // wprintf_s(L"SessionInstanceIdentifier: %s\n", instId);
            }

            AudioSessionState state;
            hr = pSessionControl->GetState(&state);

            std::string stateStr;

            switch (state) {
            case AudioSessionStateInactive:
                stateStr += "Inactive";
                // wprintf_s(L"Session state: Inactive\n", state);
                break;
            case AudioSessionStateActive:
                stateStr += "Active";
                // wprintf_s(L"Session state: Active\n", state);
                result = TRUE;
                break;
            case AudioSessionStateExpired:
                stateStr += "Expired";
                // wprintf_s(L"Session state: Expired\n", state);
                break;
            }

            printf("TEST\n");
            session new_session = { instId, stateStr };
            microphone& last_microphone = microphones.back();
            last_microphone.sessions.push_back(new_session);

            SAFE_RELEASE(pSessionControl2);
            SAFE_RELEASE(pSessionControl);
        }

        SAFE_RELEASE(pSessionList);
        SAFE_RELEASE(pSessionManager);
        SAFE_RELEASE(pProps);
        SAFE_RELEASE(pCaptureDevice);
    }

    SAFE_RELEASE(dCol);
    SAFE_RELEASE(pEnumerator);

    return microphones;
}
