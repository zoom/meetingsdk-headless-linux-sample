#include "ZoomSDKAudioRawDataDelegate.h"

void ZoomSDKAudioRawDataDelegate::onMixedAudioRawDataReceived(AudioRawData* data) {
    //cout << "onMixedAudioRawDataReceived" << endl;
}

void ZoomSDKAudioRawDataDelegate::onOneWayAudioRawDataReceived(AudioRawData* data, uint32_t node_id) {
    cout << "onOneWayAudioRawDataReceived from " << node_id << endl;
}
