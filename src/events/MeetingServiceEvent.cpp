#include "MeetingServiceEvent.h"

MeetingServiceEvent::MeetingServiceEvent(function<void()>& onMeetingJoin) : m_onMeetingJoin(onMeetingJoin) {

}
void MeetingServiceEvent::onMeetingStatusChanged(MeetingStatus status, int iResult) {
    cout << "OnMeetingStatusChanged" << endl;
    if (m_onMeetingStatusChanged) {
        m_onMeetingStatusChanged(status, iResult);
        return;
    }

    string message;
    switch (status) {
        case MEETING_STATUS_CONNECTING:
            message = "connecting to the meeting";
            break;
        case MEETING_STATUS_RECONNECTING:
            message = "reconnecting to the meeting";
            break;
        case MEETING_STATUS_DISCONNECTING:
            message = "leaving the meeting";
            break;
        case MEETING_STATUS_INMEETING:
            message = "joined meeting successfully";
            if (m_onMeetingJoin) m_onMeetingJoin();
            break;
        case MEETING_STATUS_ENDED:
            message = "joined meeting successfully";
            if (m_onMeetingEnd) m_onMeetingEnd();
            break;
        case MEETING_STATUS_FAILED:
            message = "failed to connect to the meeting";
            break;
        case MEETING_STATUS_WAITINGFORHOST:
            message = "waiting for the meeting to start";
            break;
    }

    cout << message << endl;
}

void MeetingServiceEvent::onMeetingParameterNotification(const MeetingParameter *meeting_param) {
    if (m_onMeetingParameterNotification)
        m_onMeetingParameterNotification(meeting_param);
}

void MeetingServiceEvent::onMeetingStatisticsWarningNotification(StatisticsWarningType type) {
    if (m_onMeetingStatisticsWarningNotification)
        m_onMeetingStatisticsWarningNotification(type);
}

void MeetingServiceEvent::onSuspendParticipantsActivities() {
    if (m_onSuspendParticipantsActivities)
        m_onSuspendParticipantsActivities();
}

void MeetingServiceEvent::onAICompanionActiveChangeNotice(bool bActive) {
    if (m_onAICompanionActiveChangeNotice)
        m_onAICompanionActiveChangeNotice(bActive);
}

void MeetingServiceEvent::setOnMeetingJoin(const function<void()>& callback) {
    m_onMeetingJoin = callback;
}

void MeetingServiceEvent::setOnMeetingStarted(const function<void()>& callback) {
    m_onMeetingStarted = callback;
}

void MeetingServiceEvent::setOnMeetingEnd(const function<void()>& callback) {
    m_onMeetingEnd = callback;
}

void MeetingServiceEvent::setOnMeetingParameterNotification(const function<void(const MeetingParameter*)>& callback) {
    m_onMeetingParameterNotification = callback;
}

void MeetingServiceEvent::setOnMeetingStatisticsWarningNotification(const function<void(StatisticsWarningType)>& callback) {
    m_onMeetingStatisticsWarningNotification = callback;
}

void MeetingServiceEvent::setOnSuspendParticipantsActivities(const function<void()>& callback) {
    m_onSuspendParticipantsActivities = callback;
}

void
MeetingServiceEvent::setOnAiCompanionActiveChangeNotice(const function<void(bool)>& callback) {
    m_onAICompanionActiveChangeNotice = callback;
}

void MeetingServiceEvent::setOnMeetingStatusChanged(function<void(MeetingStatus, int)> callback) {
    m_onMeetingStatusChanged = callback;
}