#include "MeetingServiceEvent.h"

MeetingServiceEvent::MeetingServiceEvent() {

}
void MeetingServiceEvent::onMeetingStatusChanged(MeetingStatus status, int iResult) {
    if (m_onMeetingStatusChanged) {
        m_onMeetingStatusChanged(status, iResult);
        return;
    }

    switch (status) {
        case MEETING_STATUS_CONNECTING:
            Log::info("connecting to the meeting");
            break;
        case MEETING_STATUS_RECONNECTING:
            Log::info("reconnecting to the meeting");
            break;
        case MEETING_STATUS_DISCONNECTING:
            Log::info("disconnecting from the meeting");
            break;
        case MEETING_STATUS_INMEETING:
            Log::success("connected");
            if (m_onMeetingJoin) m_onMeetingJoin();
            return;
        case MEETING_STATUS_ENDED:
            Log::success("meeting ended");
            if (m_onMeetingEnd) m_onMeetingEnd();
            return;
        case MEETING_STATUS_FAILED:
            Log::error("failed to connect to the meeting");
            break;
        case MEETING_STATUS_WAITINGFORHOST:
            Log::info("waiting for the meeting to start");
            break;
        default:
            Log::error("unknown meeting status");
            break;
    }
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

void MeetingServiceEvent::setOnMeetingStatusChanged(function<void(MeetingStatus, int)>& callback) {
    m_onMeetingStatusChanged = callback;
}