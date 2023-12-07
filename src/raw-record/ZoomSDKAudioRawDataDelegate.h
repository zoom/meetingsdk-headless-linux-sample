
#ifndef MEETING_SDK_LINUX_SAMPLE_ZOOMSDKAUDIORAWDATADELEGATE_H
#define MEETING_SDK_LINUX_SAMPLE_ZOOMSDKAUDIORAWDATADELEGATE_H

#include <iostream>
#include "zoom_sdk_raw_data_def.h"
#include "rawdata/rawdata_audio_helper_interface.h"

using namespace std;
using namespace ZOOMSDK;

class ZoomSDKAudioRawDataDelegate : public IZoomSDKAudioRawDataDelegate {
public:
    void onMixedAudioRawDataReceived(AudioRawData* data) override;
    void onOneWayAudioRawDataReceived(AudioRawData* data, uint32_t node_id) override;
};


#endif //MEETING_SDK_LINUX_SAMPLE_ZOOMSDKAUDIORAWDATADELEGATE_H
