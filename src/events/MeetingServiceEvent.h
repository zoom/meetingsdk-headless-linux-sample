#ifndef MEETING_SDK_LINUX_SAMPLE_MEETINGSERVICEEVENT_H
#define MEETING_SDK_LINUX_SAMPLE_MEETINGSERVICEEVENT_H

#include <iostream>
#include <string>
#include <functional>
#include "meeting_service_interface.h"

#include "../util/Log.h"

using namespace std;
using namespace ZOOMSDK;

class MeetingServiceEvent : public IMeetingServiceEvent {
    function<void()> m_onMeetingJoin;
    function<void()> m_onMeetingEnd;
    function<void(MeetingStatus status, int iResult)> m_onMeetingStatusChanged;

    function<void(const MeetingParameter* param)> m_onMeetingParameterNotification;
    function<void(StatisticsWarningType type)> m_onMeetingStatisticsWarningNotification;
    function<void()> m_onSuspendParticipantsActivities;
    function<void(bool bActive)> m_onAICompanionActiveChangeNotice;

public:
    MeetingServiceEvent();

    /**
     * Meeting status changed callback
     * @param status value of the current meeting status
     * @param iResult detailed reasons for special meeting statuses
     */
    void onMeetingStatusChanged(MeetingStatus status, int iResult) override;

    /**
     * callback will be triggered right before the meeting starts
     * The meeting_param will be destroyed once the function calls end
     * @param meeting_param holds parameters for a newly created meeting
     */
    void onMeetingParameterNotification(const MeetingParameter* meeting_param) override;

    /**
     * callback used when there are Meeting statistics warning notifications
     * @param type type of statistics warning
     */
    void onMeetingStatisticsWarningNotification(StatisticsWarningType type) override;

    /**
     * Callback event when a meeting is suspended
     */
    void onSuspendParticipantsActivities() override;

    /**
     * Callback event used when the AI Companion active status changed.
     * @param bActive true if the AI Companion is active
     */
    void onAICompanionActiveChangeNotice(bool bActive) override;


    /* Setters for Callbacks */
    void setOnMeetingJoin(const function<void()>& callback);
    void setOnMeetingEnd(const function<void()>& callback);
    void setOnMeetingStatusChanged(function<void(MeetingStatus, int)>& callback);
    void setOnMeetingParameterNotification(const function<void(const MeetingParameter*)>& callback);
    void setOnMeetingStatisticsWarningNotification(const function<void(StatisticsWarningType)>& callback);
    void setOnSuspendParticipantsActivities(const function<void()>& callback);
    void setOnAiCompanionActiveChangeNotice(const function<void(bool)>& callback);
};

#endif //MEETING_SDK_LINUX_SAMPLE_MEETINGSERVICEEVENT_H
