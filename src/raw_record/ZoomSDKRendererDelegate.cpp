#include "ZoomSDKRendererDelegate.h"

/**
void send(IZoomSDKVideoSender* sender, Frame f) {
    Log::info("sending frame");

    if (f.data)
        sender->sendVideoFrame(f.data, f.width, f.height, f.len, 0);
}
 */



ZoomSDKRendererDelegate::ZoomSDKRendererDelegate() {
    m_server.start();

    if (!m_cascade.load("/usr/share/opencv4/haarcascades/haarcascade_frontalface_alt.xml"))
        Log::error("failed to load cascade file");
}

void ZoomSDKRendererDelegate::onRawDataFrameReceived(YUVRawDataI420 *data)
{

    vector<Rect> faces;
    double scale=1;
    double fx = 1 / scale;

    Mat YV12 = cv::Mat(data->GetStreamHeight() * 3/2, data->GetStreamWidth(), CV_8UC1, data->GetBuffer());
    Mat small, gray, rgb;

    cvtColor(YV12, rgb, COLOR_YUV2RGB_YV12);
    cvtColor(YV12, gray, COLOR_YUV2GRAY_YV12);

    if (gray.empty()) return;

    // Resize the Grayscale Image
    resize( gray, small, Size(), fx, fx, INTER_LINEAR );
    equalizeHist( small, small );

thread tr1([&]() {
    // Detect faces of different sizes using cascade classifier
    m_cascade.detectMultiScale(small, faces, 1.1,
                               2, 0|CASCADE_SCALE_IMAGE, Size(30, 30) );

    stringstream msg;
    msg << "Found " << faces.size() << " faces" << endl;
    Log::info(msg.str());

    for (size_t i = 0; i < faces.size(); i++ ) {
        Rect r = faces[i];
        Scalar color = Scalar(255, 0, 0); // Color for Drawing tool

        rectangle(rgb, Point(cvRound(r.x*scale), cvRound(r.y*scale)),
                  Point(cvRound((r.x + r.width-1)*scale),
                        cvRound((r.y + r.height-1)*scale)), color, 3, 8, 0);
    }

    if (faces.size() > 0) {

        stringstream ss;
        ss << "Writing out frame " << frameCount++ << endl;
        Log::info(ss.str());

        m_server.writeBuf(rgb.data, strlen(rgb.data));


        //stringstream file;
        // file << "out/test" << frameCount << ".bmp";
        //imwrite(file.str(), rgb);
    }
});


tr1.join();

/*    YUVFrame f;
    f.width = data->GetStreamWidth();
    f.height = data->GetStreamHeight();

    unsigned int hw = f.width/2;
    unsigned int hh = f.height/2

    f.y.reserve(f.width * f.height);
    char* tmp = data->GetYBuffer();
    f.y.insert(f.y.end(), tmp, tmp + f.width * f.height)

    f.u.reserve(hw * hh);
    tmp = data->GetUBuffer();
    f.u.insert(f.u.end(), tmp, tmp + (hw * hh))

    f.v.reserve(hw * hh);
    tmp = data->GetUBuffer();
    f.v.insert(f.v.end(), tmp, tmp + (hw * hh))*/


    /*
    if (!m_videoSource->isReady()) {
        Log::info("video source not ready");
        return;
    }


    Frame f = {
            .data = data->GetBuffer(),
            .width = data->GetStreamWidth(),
            .height = data->GetStreamHeight(),
            .len = data->GetBufferLen(),
    };


    thread(send, m_videoSource->getSender(), f).detach();
*/
}

void ZoomSDKRendererDelegate::writeToFile(const string &path, YUVRawDataI420 *data)
{

	std::ofstream file(path, std::ios::out | std::ios::binary | std::ios::app);
	if (!file.is_open()) 
        return Log::error("failed to open video output file: " + path);

	file.write(data->GetBuffer(), data->GetBufferLen());

	file.close();
	file.flush();
}

void ZoomSDKRendererDelegate::setDir(const string &dir)
{
    m_dir = dir;
}

void ZoomSDKRendererDelegate::setFilename(const string &filename)
{
    m_filename = filename;
}
