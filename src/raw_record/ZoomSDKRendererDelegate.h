
#ifndef MEETING_SDK_LINUX_SAMPLE_ZOOMSDKRENDERERDELEGATE_H
#define MEETING_SDK_LINUX_SAMPLE_ZOOMSDKRENDERERDELEGATE_H

#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "zoom_sdk_raw_data_def.h"
#include "rawdata/rawdata_renderer_interface.h"

#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"


#include "../util/Log.h"


using namespace cv;
using namespace std;
using namespace ZOOMSDK;

struct YUVFrame {
    unsigned int width;
    unsigned int height;
    vector<char> y;
    vector<char> u;
    vector<char> v;

};

class ZoomSDKRendererDelegate : public IZoomSDKRendererDelegate {
    string m_dir = "out";
    string m_filename = "meeting-video.yuv";

    CascadeClassifier m_cascade;

    unsigned int frameCount = 0;
    SocketServer m_server;

public:
    ZoomSDKRendererDelegate();


    void writeToFile(const string& path, YUVRawDataI420* data);

    void setDir(const string& dir);
    void setFilename(const string& filename);

    void onRawDataFrameReceived(YUVRawDataI420* data) override;
    void onRawDataStatusChanged(RawDataStatus status) override {};
    void onRendererBeDestroyed() override {};
};


#endif //MEETING_SDK_LINUX_SAMPLE_ZOOMSDKRENDERERDELEGATE_H
