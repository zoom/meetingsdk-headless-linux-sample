#include "Zoom.h"

SDKError Zoom::config(int ac, char** av) {
    auto status = m_config.read(ac, av);
    if (status) {
        Log::error("failed to read configuration");
        return SDKERR_INTERNAL_ERROR;
    }

    return SDKERR_SUCCESS;
}

SDKError Zoom::init() { 
    InitParam initParam;

    auto host = m_config.zoomHost().c_str();

    initParam.strWebDomain = host;
    initParam.strSupportUrl = host;

    initParam.emLanguageID = LANGUAGE_English;

    initParam.enableLogByDefault = true;
    initParam.enableGenerateDump = true;

    auto err = InitSDK(initParam);
    if (hasError(err)) {
        Log::error("InitSDK failed");
        return err;
    }
    
    return createServices();
}


SDKError Zoom::createServices() {
    auto err = CreateMeetingService(&m_meetingService);
    if (hasError(err)) return err;

    err = CreateSettingService(&m_settingService);
    if (hasError(err)) return err;

    auto meetingServiceEvent = new MeetingServiceEvent();
    meetingServiceEvent->setOnMeetingJoin(onJoin);

    err = m_meetingService->SetEvent(meetingServiceEvent);
    if (hasError(err)) return err;

    return CreateAuthService(&m_authService);
}

SDKError Zoom::auth() {
    SDKError err{SDKERR_UNINITIALIZE};

    auto id = m_config.clientId();
    auto secret = m_config.clientSecret();

    if (id.empty()) {
        Log::error("Client ID cannot be blank");
        return err;
    }


    if (secret.empty()) {
        Log::error("Client Secret cannot be blank");
        return err;
    }

    err = m_authService->SetEvent(new AuthServiceEvent(onAuth));
    if (hasError(err)) return err;

    generateJWT(m_config.clientId(), m_config.clientSecret());

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
    SDKError err{SDKERR_UNINITIALIZE};

    auto mid = m_config.meetingId();
    auto password = m_config.password();
    auto displayName = m_config.displayName();


    if (mid.empty()) {
        Log::error("Meeting ID cannot be blank");
        return err;
    }

    if (password.empty()) {
        Log::error("Meeting Password cannot be blank");
        return err;
    }

    if (displayName.empty()) {
        Log::error("Display Name cannot be blank");
        return err;
    }

    auto meetingNumber = stoull(mid);
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

    if (!m_config.zak().empty()) {
        Log::success("used ZAK token");
        param.userZAK = m_config.zak().c_str();
    }

    if (!m_config.joinToken().empty()) {
        Log::success("used App Privilege token");
        param.app_privilege_token = m_config.joinToken().c_str();
    }

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

    delete m_renderDelegate;
    return CleanUPSDK();
}

SDKError Zoom::startRawRecording() {
    auto recCtl = m_meetingService->GetMeetingRecordingController();

    SDKError err = recCtl->CanStartRawRecording();

    if (hasError(err)) {
        Log::info("requesting local recording privilege");
        return recCtl->RequestLocalRecordingPrivilege();
    }

    err = recCtl->StartRawRecording();
    if (hasError(err, "start raw recording"))
        return err;

    if (m_config.useRawVideo()) {
        if (!m_renderDelegate) {
            m_renderDelegate = new ZoomSDKRendererDelegate();
            m_videoSource = new ZoomSDKVideoSource();
        }

        err = createRenderer(&m_videoHelper, m_renderDelegate);
        if (hasError(err, "create raw video renderer"))
            return err;

        m_renderDelegate->setDir(m_config.videoDir());
        m_renderDelegate->setFilename(m_config.videoFile());
        
        auto participantCtl = m_meetingService->GetMeetingParticipantsController();
        auto uid = participantCtl->GetParticipantsList()->GetItem(0);

        m_videoHelper->setRawDataResolution(ZoomSDKResolution_720P);
        err = m_videoHelper->subscribe(uid, RAW_DATA_TYPE_VIDEO);
        if (hasError(err, "subscribe to raw video"))
            return err;

  /*      auto* videoSourceHelper = GetRawdataVideoSourceHelper();
        if (!videoSourceHelper) {
            Log::error("Initializing Video Source Helper");
            return SDKERR_UNINITIALIZE;
        }

        err = videoSourceHelper->setExternalVideoSource(m_videoSource);
        if (hasError(err, "set video source"))
            return err;

        auto* videoSettings = m_settingService->GetVideoSettings();
        videoSettings->EnableAutoTurnOffVideoWhenJoinMeeting(false);

       auto* sender = m_videoSource->getSender();
        SDKError e;
        do {
            Log::info("attempting unmute");
            auto* videoCtl = m_meetingService->GetMeetingVideoController();
            e = videoCtl->UnmuteVideo();
            if (hasError(e, "unmute")) sleep(1);
        } while (hasError(e));*/

    }

    if (m_config.useRawAudio()) {
        m_audioHelper = GetAudioRawdataHelper();
        if (!m_audioHelper)
            return SDKERR_UNINITIALIZE;

        if (!m_audioSource) {
            auto mixedAudio = !m_config.separateParticipantAudio();
            auto transcribe = m_config.transcribe();

            m_audioSource = new ZoomSDKAudioRawDataDelegate(mixedAudio, transcribe);
            m_audioSource->setDir(m_config.audioDir());
            m_audioSource->setFilename(m_config.audioFile());
        }

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


bool Zoom::hasError(const SDKError e, const string& action) {
    auto isError = e != SDKERR_SUCCESS;

    if(!action.empty()) {
        if (isError) {
            stringstream ss;
            ss << "failed to " << action << " with status " << e;
            Log::error(ss.str());
        } else {
            Log::success(action);
        }
    }
    return isError;
}
