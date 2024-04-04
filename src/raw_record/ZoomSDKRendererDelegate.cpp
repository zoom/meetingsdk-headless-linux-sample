#include "ZoomSDKRendererDelegate.h"

void ZoomSDKRendererDelegate::onRawDataFrameReceived(YUVRawDataI420 *data)
{
    stringstream path;
    path << m_dir << "/" << m_filename;

    writeToFile(path.str(), data);
}

void ZoomSDKRendererDelegate::writeToFile(const string &path, YUVRawDataI420 *data)
{

	std::ofstream file(path, std::ios::out | std::ios::binary | std::ios::app);
	if (!file.is_open()) 
        return Log::error("failed to open video output file: " + path);
	
	// Calculate the sizes for Y, U, and V components
	size_t ySize = data->GetStreamWidth() * data->GetStreamHeight();
	size_t uvSize = ySize / 4;

	// Write Y, U, and V components to the output file
	file.write(data->GetYBuffer(), ySize);
	file.write(data->GetUBuffer(), uvSize);
	file.write(data->GetVBuffer(), uvSize);

    auto bytes = ySize + uvSize*2;
    stringstream ss;
    ss << "Writing " << bytes << "b to " << path;

    Log::info(ss.str());

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
