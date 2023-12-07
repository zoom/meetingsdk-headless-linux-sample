#include "Config.h"

Config::Config() :
        m_app(m_name, m_version),
        m_rawRecordCmd(m_app.add_subcommand("RawRecord", "Begin Raw Recording A/V for use as a Zoom Bot")) {

    m_app.set_config("--config")->default_str("config.ini");

    m_app.add_option("-m, --meeting-id", m_meetingId,"Meeting ID of the meeting");
    m_app.add_option("-p, --password", m_password,"Password of the meeting");
    m_app.add_option("-n, --display-name",m_displayName,"Display Name for the meeting");

    m_app.add_option("--host", m_zoomHost, "Host Domain for the Zoom Meeting")->default_str("https://zoom.us");
    m_app.add_option("-u, --url", m_joinUrl, "Join or Start a Meeting URL");
    m_app.add_option("-t, --join-token", m_joinToken, "Join the meeting with App Privilege using a token");

    m_app.add_option("--sdk-key", m_sdkKey, "Zoom Meeting SDK Key");
    m_app.add_option("--sdk-secret", m_sdkSecret, "Zoom Meeting SDK Secret");

    m_app.add_flag("-s, --start", m_isMeetingStart, "Start a Zoom Meeting");

    m_rawRecordCmd->add_flag("-v, --video", m_useRawVideo, "Record Raw Video Data");
    m_rawRecordCmd->add_flag("-a, --audio", m_useRawAudio, "Record Raw Audio Data");
}

int Config::read(int ac, char **av) {
    try {
        m_app.parse(ac, av);
    } catch (const CLI::ParseError& err) {
        return m_app.exit(err);
    }

    if (!m_joinUrl.empty())
        parseUrl(m_joinUrl);

   return 0;
}

const string& Config::sdkKey() const {
    return m_sdkKey;
}

const string& Config::sdkSecret() const {
    return m_sdkSecret;
}

bool Config::parseUrl(const string& join_url) {
    auto url = ada::parse<ada::url>(join_url);

    if (!url) {
        cerr << "unable to parse join URL" << endl;
        return false;
    }

    string token, lastRoute;
    istringstream ss(static_cast<string>(url->get_pathname()));

    while (getline(ss, token, '/')) {
        if(token.empty()) continue;

        m_isMeetingStart = token == "s";

        if (lastRoute == "j" || lastRoute == "s") {
            m_meetingId = token;
            break;
        }

        lastRoute = token;
    }

    if (m_meetingId.empty()) return false;

    ada::url_search_params search(url->get_search());
    if (!search.has("pwd")) return false;
    m_password = move(*search.get(string_view("pwd")));

    return true;
}

bool Config::useRawRecording() const {
    return m_rawRecordCmd->parsed();
}

bool Config::useRawAudio() const {
    return m_useRawAudio;
}

bool Config::useRawVideo() const {
    return m_useRawVideo;
}

bool Config::isMeetingStart() const {
    return m_isMeetingStart;
}

const string& Config::joinToken() const {
    return m_joinToken;
}

const string& Config::meetingId() const {
    return m_meetingId;
}

const string& Config::password() const {
    return m_password;
}

const string& Config::displayName() const {
    return m_displayName;
}

const string& Config::zoomHost() const {
    return m_zoomHost;
}

