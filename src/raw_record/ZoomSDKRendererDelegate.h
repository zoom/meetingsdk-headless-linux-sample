
#ifndef MEETING_SDK_LINUX_SAMPLE_ZOOMSDKRENDERERDELEGATE_H
#define MEETING_SDK_LINUX_SAMPLE_ZOOMSDKRENDERERDELEGATE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include "zoom_sdk_raw_data_def.h"
#include "rawdata/rawdata_renderer_interface.h"

#include "../util/Log.h"

using namespace std;
using namespace ZOOMSDK;

class ZoomSDKRendererDelegate : public IZoomSDKRendererDelegate {
    string m_dir = "out";
    string m_filename = "meeting-video.yuv";
public:
    void writeToFile(const string& path, YUVRawDataI420* data);

    void setDir(const string& dir);
    void setFilename(const string& filename);

    void onRawDataFrameReceived(YUVRawDataI420* data) override;
    void onRawDataStatusChanged(RawDataStatus status) override {};
    void onRendererBeDestroyed() override {};
};


#endif //MEETING_SDK_LINUX_SAMPLE_ZOOMSDKRENDERERDELEGATE_H
