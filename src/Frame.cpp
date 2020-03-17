#include "Frame.h"

// init static variable
int Frame::_frameCnt = 0;

Frame::Frame(){
    _frameNumber = ++_frameCnt;
}

std::future<void> Frame::process(){
    //std::cout << "Frame # " << _frameNumber << " ::process: thread frame = " << std::this_thread::get_id() << std::endl;
    // initalize variables

    // push frame to the queue
    std::future<void> infeResultFuture;
    auto ftrFrameProcessed = std::async(std::launch::async, &FrameProcessing::addFrameToQueue,
                                                       _frameProcessing, get_shared_this(), std::move(infeResultFuture));

    // wait until frame has been pushed to the queue
    return ftrFrameProcessed.get();
}

void Frame::setFrameBuffer(cv::Mat &&frame){
    cv::imencode(".jpg", frame, _frameBuffer);
}
