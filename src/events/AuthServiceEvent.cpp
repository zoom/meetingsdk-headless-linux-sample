#include "AuthServiceEvent.h"

AuthServiceEvent::AuthServiceEvent(function<void()>& onAuth) : m_onAuth(onAuth) {}

void AuthServiceEvent::onAuthenticationReturn(AuthResult result) {
    if (m_onAuthenticationReturn) {
        m_onAuthenticationReturn(result);
        return;
    }

    stringstream message;
    message << "authentication failed because the ";

    switch (result) {
        case AUTHRET_KEYORSECRETEMPTY:
            message << "key or secret is empty";
            break;
        case AUTHRET_JWTTOKENWRONG:
            message << "JWT is invalid";
            break;
        case AUTHRET_OVERTIME:
            message << "operation timed out";
            break;
        case AUTHRET_SUCCESS:
            if (m_onAuth) m_onAuth();
            else message << "authentication callback was not set";
            break;
        default:
            message << "Zoom SDK encountered an unknown error: " << result;
            break;
    }

    if (result != AUTHRET_SUCCESS) {
        cerr << message.str() << endl;
        abort();
    }

    return;
}

void AuthServiceEvent::onLogout() {
    if (m_onLogout)
        m_onLogout();
}

void AuthServiceEvent::onZoomIdentityExpired() {
    if (m_onZoomIdentityExpired)
        m_onZoomIdentityExpired();
}

void AuthServiceEvent::onZoomAuthIdentityExpired() {
    if (m_onZoomAuthIdentityExpired)
        m_onZoomAuthIdentityExpired();
}

void AuthServiceEvent::onLoginReturnWithReason(LOGINSTATUS ret, IAccountInfo *pAccountInfo, LoginFailReason reason) {
    if (m_onLoginReturnWithReason)
        m_onLoginReturnWithReason(ret, pAccountInfo, reason);
}

void AuthServiceEvent::setOnAuthenticationReturn(const function<void(AuthResult)>& callback) {
    m_onAuthenticationReturn = callback;
}

void AuthServiceEvent::setOnLogout(const function<void()>& callback) {
    m_onLogout = callback;
}

void AuthServiceEvent::setOnZoomIdentityExpired(const function<void()>& callback) {
    m_onZoomIdentityExpired = callback;
}

void AuthServiceEvent::setOnZoomAuthIdentityExpired(const function<void()>& callback) {
    m_onZoomAuthIdentityExpired = callback;
}

void AuthServiceEvent::setOnLoginReturnWithReason(const function<void(LOGINSTATUS, IAccountInfo*, LoginFailReason)>& callback) {
    m_onLoginReturnWithReason = callback;
}

void AuthServiceEvent::setOnAuth(const function<void()> callback) {
    m_onAuth = callback;
}