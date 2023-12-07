//
// Created by Max Mansfield on 8/30/23.
//

#ifndef MEETING_SDK_LINUX_SAMPLE_CONFIG_H
#define MEETING_SDK_LINUX_SAMPLE_CONFIG_H

#include <iostream>
#include <fstream>
#include <iterator>

#include <codecvt>
#include <algorithm>
#include <locale>
#include <string>

#include <ada.h>

#include <CLI/CLI.hpp>

using namespace std;
using namespace ada;

class Config {

    const string& m_name = "Zoom Meeting SDK for Linux Sample";
    const string& m_version = "0.1.0";

    CLI::App m_app;

    CLI::App* m_rawRecordCmd;
    bool m_useRawAudio;
    bool m_useRawVideo;

    bool m_isMeetingStart;

    string m_joinUrl;
    string m_meetingId;
    string m_password;
    string m_displayName;

    string m_sdkKey;
    string m_sdkSecret;

    string m_zoomHost;
    string m_joinToken;

public:
    Config();

    int read(int ac, char **av);

    bool parseUrl(const string& join_url);

    const string& meetingId() const;
    const string& password() const;
    const string& displayName() const;

    const string& joinToken() const;
    const string& joinUrl() const;

    const string& sdkKey() const;
    const string& sdkSecret() const;

    const string& zoomHost() const;

    bool isMeetingStart() const;
    bool useRawRecording() const;
    bool useRawAudio() const;
    bool useRawVideo() const;
};


#endif //MEETING_SDK_LINUX_SAMPLE_CONFIG_H
