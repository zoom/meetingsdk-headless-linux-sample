#include "MeetingRecordingCtrlEvent.h"

MeetingRecordingCtrlEvent::MeetingRecordingCtrlEvent(function<void(bool)> onPrivilegeChanged) : m_onRecordingPrivilegeChanged(onPrivilegeChanged) {
}


MeetingRecordingCtrlEvent::~MeetingRecordingCtrlEvent() {}

void MeetingRecordingCtrlEvent::onRecordPrivilegeChanged(bool bCanRec) {
    if (m_onRecordingPrivilegeChanged)
        m_onRecordingPrivilegeChanged(bCanRec);
}

void MeetingRecordingCtrlEvent::onRecordingStatus(RecordingStatus status){}

void MeetingRecordingCtrlEvent::onCloudRecordingStatus(RecordingStatus status) {}

void MeetingRecordingCtrlEvent::onLocalRecordingPrivilegeRequestStatus(RequestLocalRecordingStatus status) {}

void MeetingRecordingCtrlEvent::onLocalRecordingPrivilegeRequested(IRequestLocalRecordingPrivilegeHandler* handler) {}

void MeetingRecordingCtrlEvent::onCloudRecordingStorageFull(time_t gracePeriodDate){}
