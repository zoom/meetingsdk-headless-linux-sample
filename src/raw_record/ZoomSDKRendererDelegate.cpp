#include "ZoomSDKRendererDelegate.h"


ZoomSDKRendererDelegate::ZoomSDKRendererDelegate() {
    // For X11 Forwarding
    XInitThreads();

    if (!m_cascade.load("/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml"))
        Log::error("failed to load cascade file");

    m_faces.reserve(2);
    m_socketServer.start();
}

void ZoomSDKRendererDelegate::onRawDataFrameReceived(YUVRawDataI420 *data)
{
    auto res = async(launch::async, [&]{
        Mat I420(data->GetStreamHeight() * 3/2, data->GetStreamWidth(), CV_8UC1, data->GetBuffer());
        Mat small, gray;

        cvtColor(I420, gray, COLOR_YUV2GRAY_I420);

        resize(gray, small, Size(), m_fx, m_fx, INTER_LINEAR);
        equalizeHist(small, small);

        m_cascade.detectMultiScale(small, m_faces, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(30, 30));

        stringstream ss;
        ss << m_faces.size();
        m_socketServer.writeStr(ss.str());

        if (m_frameCount++ % 2 == 0) {
            Scalar color = Scalar(0, 0, 255);
            for (size_t i = 0; i < m_faces.size(); i++) {
                Rect r = m_faces[i];
                rectangle( gray, Point(cvRound(r.x*m_scale), cvRound(r.y*m_scale)),
                           Point(cvRound((r.x + r.width-1)*m_scale),
                                   cvRound((r.y + r.height-1)*m_scale)), color, 3, 8, 0);
            }

            imshow(c_window, gray);
        }
    });
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
