#include "ZoomSDKRendererDelegate.h"

void ZoomSDKRendererDelegate::onRawDataFrameReceived(YUVRawDataI420* data) {
    // write each frame to stdout
    cout << data->GetYBuffer() << data->GetUBuffer() << data->GetVBuffer();
}

void ZoomSDKRendererDelegate::onRawDataStatusChanged(RawDataStatus status) {
    cout << "onRawDataStatusChanged() " << status << endl;
}

void ZoomSDKRendererDelegate::onRendererBeDestroyed() {
    cout << "onRendererBeDestroyed() " << endl;
}
