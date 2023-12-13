#include "Zoom.h"

SDKError Zoom::config(int ac, char** av) {
    auto status = m_config.read(ac, av);
    if (status) {
        cerr << "failed to read configuration" << endl;
        return SDKERR_INTERNAL_ERROR;
    }

    return SDKERR_SUCCESS;
}

SDKError Zoom::init() {
    SDKError err;

    auto key = m_config.sdkKey();
    auto secret = m_config.sdkSecret();

    if (key.empty() || secret.empty()) {
        return SDKERR_UNINITIALIZE;
    }

    InitParam initParam;

    auto host = m_config.zoomHost().c_str();

    initParam.strWebDomain = host;
    initParam.strSupportUrl = host;

    initParam.emLanguageID = LANGUAGE_English;

    initParam.enableLogByDefault = true;
    initParam.enableGenerateDump = true;

    err = InitSDK(initParam);
    if (hasError(err)) return err;

    return createServices();
}


SDKError Zoom::createServices() {
    auto err = CreateMeetingService(&m_meetingService);
    if (hasError(err)) return err;

    err = CreateSettingService(&m_settingService);
    if (hasError(err)) return err;

    function<void()> onJoin = [&]() {
        auto* reminderController = m_meetingService->GetMeetingReminderController();
        reminderController->SetEvent(new MeetingReminderEvent());

        if (m_config.useRawRecording()) {

            auto recordingCtrl = m_meetingService->GetMeetingRecordingController();

            function<void(bool)> onRecordingPrivilegeChanged = [&](bool canRec) {
                if (canRec)
                    startRawRecording();
                else
                    stopRawRecording();
            };

            auto recordingEvent = new MeetingRecordingCtrlEvent(onRecordingPrivilegeChanged);
            recordingCtrl->SetEvent(recordingEvent);

            auto e = recordingCtrl->CanStartRawRecording();
            string action = " local recording privilege";

            if (e == SDKERR_SUCCESS) {
                e = startRawRecording();
                action = "has" + action;
            } else {
                e = recordingCtrl->RequestLocalRecordingPrivilege();
                action = "request" + action;
            }

            hasError(e, action);
        }
    };

    auto meetingServiceEvent = new MeetingServiceEvent();
    meetingServiceEvent->setOnMeetingJoin(onJoin);

    err = m_meetingService->SetEvent(meetingServiceEvent);
    return err;
}

SDKError Zoom::auth() {
   SDKError err;

    err = CreateAuthService(&m_authService);
    if (hasError(err)) return err;

    function<void()> onAuth = [&]() {
        auto err = startOrJoin();
        if(hasError(err)) {
            error("failed to start or join a meeting");
            exit(err);
        }
    };

    err = m_authService->SetEvent(new AuthServiceEvent(onAuth));
    if (hasError(err)) return err;

    generateJWT(m_config.sdkKey(), m_config.sdkSecret());

    AuthContext ctx;
    ctx.jwt_token =  m_jwt.c_str();

    return m_authService->SDKAuth(ctx);
}

void Zoom::generateJWT(const string& key, const string& secret) {

    m_iat = std::chrono::system_clock::now();
    m_exp = m_iat + std::chrono::hours{24};

    m_jwt = jwt::create()
            .set_type("JWT")
            .set_issued_at(m_iat)
            .set_expires_at(m_exp)
            .set_payload_claim("appKey", claim(key))
            .set_payload_claim("tokenExp", claim(m_exp))
            .sign(algorithm::hs256{secret});
}

SDKError Zoom::join() {
    auto id = m_config.meetingId();
    auto password = m_config.password();
    auto displayName = m_config.displayName();

    if (id.empty() || password.empty()) {
        cerr << "you must provide an id and password to join a meeting" << endl;
        return SDKERR_UNINITIALIZE;
    }

    auto meetingNumber = stoull(id);
    auto userName = displayName.c_str();
    auto psw = password.c_str();

    JoinParam joinParam;
    joinParam.userType = ZOOM_SDK_NAMESPACE::SDK_UT_WITHOUT_LOGIN;

    JoinParam4WithoutLogin& param = joinParam.param.withoutloginuserJoin;

    param.meetingNumber = meetingNumber;
    param.userName = userName;
    param.psw = psw;
    param.vanityID = nullptr;
    param.customer_key = nullptr;
    param.webinarToken = nullptr;
    param.isVideoOff = false;
    param.isAudioOff = false;

    if (!m_config.joinToken().empty())
        param.app_privilege_token = m_config.joinToken().c_str();

    if (m_config.useRawAudio()) {
        auto* audioSettings = m_settingService->GetAudioSettings();
        if (!audioSettings) return SDKERR_INTERNAL_ERROR;

        audioSettings->EnableAutoJoinAudio(true);
    }

    return m_meetingService->Join(joinParam);
}

SDKError Zoom::start() {
    SDKError err;

    StartParam startParam;
    startParam.userType = SDK_UT_NORMALUSER;

    StartParam4NormalUser  normalUser;
    normalUser.vanityID = nullptr;
    normalUser.customer_key = nullptr;
    normalUser.isAudioOff = false;
    normalUser.isVideoOff = false;

    err = m_meetingService->Start(startParam);
    hasError(err, "start meeting");

    return err;
}

SDKError Zoom::leave() {
    if (!m_meetingService)
        return SDKERR_UNINITIALIZE;

    auto status = m_meetingService->GetMeetingStatus();
    if (status == MEETING_STATUS_IDLE)
        return SDKERR_WRONG_USAGE;

    return  m_meetingService->Leave(LEAVE_MEETING);
}

SDKError Zoom::clean() {
    if (m_meetingService)
        DestroyMeetingService(m_meetingService);

    if (m_settingService)
        DestroySettingService(m_settingService);

    if (m_authService)
        DestroyAuthService(m_authService);

    if (m_audioHelper)
        m_audioHelper->unSubscribe();

    if (m_videoHelper)
        m_videoHelper->unSubscribe();

    return CleanUPSDK();
}

SDKError Zoom::startOrJoin() {
    SDKError err;

    if (isMeetingStart())
        err = start();
    else
        err = join();

    return err;
}

SDKError Zoom::startRawRecording() {
    SDKError err;

    auto recCtrl = m_meetingService->GetMeetingRecordingController();
    if (recCtrl->CanStartRawRecording() != SDKERR_SUCCESS)
        return SDKERR_UNAUTHENTICATION;

    err = recCtrl->StartRawRecording();
    if (hasError(err, "start raw recording"))
        return err;

    if (m_config.useRawVideo()) {
        err = createRenderer(&m_videoHelper, m_videoSource);
        if (hasError(err, "create raw video renderer"))
            return err;

        auto participantCtl = m_meetingService->GetMeetingParticipantsController();
        int uid = participantCtl->GetParticipantsList()->GetItem(0);

        m_videoHelper->setRawDataResolution(ZoomSDKResolution_720P);
        err = m_videoHelper->subscribe(uid, RAW_DATA_TYPE_VIDEO);
        if (hasError(err, "subscribe to raw video"))
            return err;
    }

    if (m_config.useRawAudio()) {
        m_audioHelper = GetAudioRawdataHelper();
        if (!m_audioHelper)
            return SDKERR_UNINITIALIZE;

        if (!m_audioSource)
            m_audioSource = new ZoomSDKAudioRawDataDelegate();

        err = m_audioHelper->subscribe(m_audioSource);
        if (hasError(err, "subscribe to raw audio"))
            return err;
    }

    return SDKERR_SUCCESS;
}

SDKError Zoom::stopRawRecording() {
    auto recCtrl = m_meetingService->GetMeetingRecordingController();
    auto err = recCtrl->StopRawRecording();
    hasError(err, "stop raw recording");

    return err;
}

bool Zoom::isMeetingStart() {
    return m_config.isMeetingStart();
}

void Zoom::success(const string& message) {
    cout << Emoji::checkMark << " " << message << endl;
}

void Zoom::info(const std::string& message) {
    cout << Emoji::hourglass << " " << message << endl;

}

void Zoom::error(const string& message) {
    cerr << Emoji::crossMark << " " << message << endl;
}

bool Zoom::hasError(const SDKError e, const string& action) {
    auto isError = e != SDKERR_SUCCESS;

    if(!action.empty()) {
        if (isError) {
            stringstream ss;
            ss << "failed to " << action << " with status " << e;
            Zoom::error(ss.str());
        } else {
            Zoom::success(action);
        }
    }
    return isError;
}


