
#ifndef MEETING_SDK_LINUX_SAMPLE_ZOOMSDKRENDERERDELEGATE_H
#define MEETING_SDK_LINUX_SAMPLE_ZOOMSDKRENDERERDELEGATE_H

#include <iostream>
#include "zoom_sdk_raw_data_def.h"
#include "rawdata/rawdata_renderer_interface.h"

using namespace std;
using namespace ZOOMSDK;

class ZoomSDKRendererDelegate : public IZoomSDKRendererDelegate {
public:
    void onRawDataFrameReceived(YUVRawDataI420* data) override;
    void onRawDataStatusChanged(RawDataStatus status) override;
    void onRendererBeDestroyed() override;
};


#endif //MEETING_SDK_LINUX_SAMPLE_ZOOMSDKRENDERERDELEGATE_H
