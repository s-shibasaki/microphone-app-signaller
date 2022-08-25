#pragma once
#include <iostream>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <functiondiscoverykeys_devpkey.h>
#include <vector>

struct session {
    LPWSTR instId;
    std::string state;
};

struct microphone {
    LPWSTR friendlyName;
    std::vector<session> sessions;
};

std::vector<microphone> GetMicrophones(void);