#include "DetectAppUsingMicrophone.h"

int main()
{
    std::vector<microphone> microphones = GetMicrophones();
    for (long i = 0; i < microphones.size(); i++) {
        printf("Friendly name: %S\n", microphones[i].friendlyName);
        for (long j = 0; j < microphones[i].sessions.size(); j++) {
            printf("  Session instance identifier: %S\n", microphones[i].sessions[j].instId);
            printf("  Session state: %s\n", microphones[i].sessions[j].state.c_str());
        }
    }
    return 0;
}

