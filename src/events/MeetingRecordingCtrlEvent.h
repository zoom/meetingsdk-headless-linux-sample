
#ifndef MEETING_SDK_LINUX_SAMPLE_MEETINGRECORDCTRLEVENT_H
#define MEETING_SDK_LINUX_SAMPLE_MEETINGRECORDCTRLEVENT_H

#include <iostream>
#include <functional>
#include "meeting_service_components/meeting_recording_interface.h"

using namespace std;
using namespace ZOOMSDK;

class MeetingRecordingCtrlEvent : public IMeetingRecordingCtrlEvent {

    function<void(bool)> m_onRecordingPrivilegeChanged;

public:
    MeetingRecordingCtrlEvent(function<void(bool)> onPrivilegeChanged);
    ~MeetingRecordingCtrlEvent();

    /**
     * Fires when the status of local recording changes
     * @param status local recording status
     */
    void onRecordingStatus(RecordingStatus status) override;

    /**
     * Fires when the status of cloud recording changes
     * @param status cloud recording status
     */
    void onCloudRecordingStatus(RecordingStatus status) override;

    /**
     * Fires when recording privilege changes
     * @param bCanRec true if the user can record
     */
    void onRecordPrivilegeChanged(bool bCanRec) override;

    /**
     * fires when the local recording privilege changes
     * @param status status of the local recording privliege request
     */
    void onLocalRecordingPrivilegeRequestStatus(RequestLocalRecordingStatus status) override;

    /**
     * Fires when a user requests local recording privilege
     * @param handler data when local recording privilege is requested
     */
    void onLocalRecordingPrivilegeRequested(IRequestLocalRecordingPrivilegeHandler* handler) override;

    /**
     * Fires when Cloud Storage is full
     * @param gracePeriodDate UTC milliseconds grace period to continue using storage
    */
    void onCloudRecordingStorageFull(time_t gracePeriodDate) override;

};


#endif //MEETING_SDK_LINUX_SAMPLE_MEETINGRECORDCTRLEVENT_H
