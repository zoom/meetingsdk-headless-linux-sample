#include "MeetingRecordingCtrlEvent.h"

MeetingRecordingCtrlEvent::MeetingRecordingCtrlEvent(function<void(bool)> onPrivilegeChanged) : m_onRecordingPrivilegeChanged(onPrivilegeChanged) {

}


MeetingRecordingCtrlEvent::~MeetingRecordingCtrlEvent() {

}

void MeetingRecordingCtrlEvent::onRecordPrivilegeChanged(bool bCanRec) {
    if (m_onRecordingPrivilegeChanged)
        m_onRecordingPrivilegeChanged(bCanRec);
}

void MeetingRecordingCtrlEvent::onRecordingStatus(RecordingStatus status){
    cout << "Recording Status " << status << endl;
}

void MeetingRecordingCtrlEvent::onCloudRecordingStatus(RecordingStatus status) {
    cout << "Cloud Recording Status " << status << endl;
}

void MeetingRecordingCtrlEvent::onLocalRecordingPrivilegeRequestStatus(RequestLocalRecordingStatus status) {
    cout << "Local Recording Privilege Request Status " << status << endl;
}

void MeetingRecordingCtrlEvent::onLocalRecordingPrivilegeRequested(IRequestLocalRecordingPrivilegeHandler* handler) {
    cout << "Local Recording Privilege Requested " << endl;
}


