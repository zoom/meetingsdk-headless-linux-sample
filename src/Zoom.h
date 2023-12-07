#ifndef MEETING_SDK_LINUX_SAMPLE_ZOOM_H
#define MEETING_SDK_LINUX_SAMPLE_ZOOM_H

#include <iostream>
#include <chrono>
#include <string>
#include <sstream>

#include <jwt-cpp/jwt.h>

#include "Config.h"

#include "zoom_sdk.h"
#include "rawdata/zoom_rawdata_api.h"
#include "rawdata/rawdata_renderer_interface.h"

#include "meeting_service_components/meeting_audio_interface.h"
#include "meeting_service_components/meeting_participants_ctrl_interface.h"

#include "events/AuthServiceEvent.h"
#include "events/MeetingServiceEvent.h"
#include "events/MeetingReminderEvent.h"
#include "events/MeetingRecordingCtrlEvent.h"

#include "raw-record/ZoomSDKRendererDelegate.h"
#include "raw-record/ZoomSDKAudioRawDataDelegate.h"

using namespace std;
using namespace jwt;
using namespace ZOOMSDK;

typedef chrono::time_point<chrono::system_clock> time_point;

class Zoom {
    Config* m_config;

    string m_jwt;

    time_point m_iat;
    time_point m_exp;

    IMeetingService* m_meetingService;
    IAuthService* m_authService;

    IZoomSDKRenderer* m_videoHelper;
    ZoomSDKRendererDelegate* m_videoSource;

    IZoomSDKAudioRawDataHelper* m_audioHelper;
    ZoomSDKAudioRawDataDelegate* m_audioSource;

    SDKError startRawRecording();
    SDKError stopRawRecording();
    void generateJWT(const string& key, const string& secret);

public:
    Zoom(Config* config);

    SDKError init();
    SDKError auth(function<void()> onAuth);

    SDKError join();
    SDKError start();
    SDKError leave();

    SDKError clean();

    SDKError startOrJoin();

    bool isMeetingStart();
    static bool hasError(SDKError e, const string& action="") ;
};

#endif //MEETING_SDK_LINUX_SAMPLE_ZOOM_H
