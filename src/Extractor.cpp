#include <iostream>
#include "Extractor.h"

/* Implementation of class "WaitingFrameFutures" */
void WaitingFrameFutures::processFirstInQueue()
{
    std::unique_lock<std::mutex> lock(_mutexFrameFutures);
    if (_frameFutures.size() > 0 ){
        auto futureTmp = _frameFutures.begin();
        //wait for inference
        futureTmp->second.get();
        Frame* frame = futureTmp->first.get();
        std::cout << "Extractor Frame # " <<  frame->getFrameNumber() << " processed!" << std::endl;
        //Remove future from vector
        _frameFutures.erase(_frameFutures.begin());
    }
}

void WaitingFrameFutures::pushBack(std::pair<std::shared_ptr<Frame>, std::future<void>> &&pair)
{
    std::lock_guard<std::mutex> lock(_mutexFrameFutures);

    _frameFutures.push_back(std::move(pair));
}

Extractor::Extractor(){
    _processingFrameFutures = false;
}

void Extractor::loadVideo()
{
    _cap = cv::VideoCapture(this->_videoName);
}

void Extractor::setVideoName(std::string videoName){
    _videoName = videoName;
}

//this a thread to waits for each processed frame
void Extractor::processFutures(){
    while (true) {
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        // wait for future response here
        if(_processingFrameFutures){
            _waitingFrameFutures.processFirstInQueue();
        }
    }
}

void Extractor::extractVideo()
{
    std::shared_ptr<FrameProcessing> processingPtr = std::make_shared<FrameProcessing>();
    processingPtr->startProcessing();
    while (_cap.isOpened())
    {
        std::shared_ptr<Frame> framePtr = std::make_shared<Frame>();
        cv::Mat frame;
        if(_cap.read(frame)){
            //send frame to queue
            framePtr->setFrameBuffer(std::move(frame));
            framePtr->setFrameProcessing(processingPtr);
            std::future<void> frameFuture = framePtr->process();
            std::pair<std::shared_ptr<Frame>, std::future<void>> framePair(framePtr, std::move(frameFuture));
            _waitingFrameFutures.pushBack(std::move(framePair));
            if(!_processingFrameFutures){
                //launch frame-future processing in batch
                _threads.emplace_back(std::thread(&Extractor::processFutures, this));
                _processingFrameFutures = true;
            }
        }
        else{
            _cap.release();
        }
    }
    std::cout << "[DONE] Processing video " << _videoName << std::endl;
}
