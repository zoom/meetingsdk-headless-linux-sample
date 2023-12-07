#include "ZoomSDKAudioRawDataDelegate.h"


void ZoomSDKAudioRawDataDelegate::onMixedAudioRawDataReceived(AudioRawData* data) {
    //  redirect program output to a file to save data
    cout << data->GetBuffer();
}

void ZoomSDKAudioRawDataDelegate::onOneWayAudioRawDataReceived(AudioRawData* data, uint32_t node_id) {
    cout << "onOneWayAudioRawDataReceived from " << node_id << endl;
}
