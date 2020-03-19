#include "FrameProcessing.h"

const unsigned int FrameProcessing::_sagemakerCallLimit = 1000;
unsigned int FrameProcessing::_sagemakerCallCount;

/* Implementation of class "WaitingFrames" */
int WaitingFrames::getSize()
{
    std::lock_guard<std::mutex> lck(_mutex);

    return _frames.size();
}

void WaitingFrames::processFirstInQueue(std::shared_ptr<Frame> &frame, std::promise<void> &promise)
{
    std::unique_lock<std::mutex> lck(_mutex);

    // get entries from the front of both queues
    auto firstFrame = _frames.begin();
    auto firstPromise = _promises.begin();
    firstFrame->swap(frame);
    firstPromise->swap(promise);
    _frames.erase(firstFrame);
    _promises.erase(firstPromise);
}

void WaitingFrames::pushBack(std::shared_ptr<Frame> frame, std::promise<void> &&promise)
{
    std::lock_guard<std::mutex> lock(_mutex);

    _frames.emplace_back(frame);
    _promises.emplace_back(std::move(promise));
}

void WaitingSageResponse::pushBack(std::future<void> &&future, std::promise<void> &&framePromise)
{
    std::lock_guard<std::mutex> lck(_mutexSage);

    _futures.emplace_back(std::move(future));
    _framePromises.emplace_back(std::move(framePromise));
}

void WaitingSageResponse::waitForFirstInQueue(bool &frameProcessed)
{
    std::lock_guard lck(_mutexSage);
    if(_futures.size() > 0 && _framePromises.size() > 0){
        auto future = _futures.begin();
        auto framePromise = _framePromises.begin();
        //Wait for Sagemaker
        future->get();
        //Fulfill frame promise
        framePromise->set_value();
        _futures.erase(_futures.begin());
        _framePromises.erase(_framePromises.begin());
        frameProcessed = true;
    }
}

FrameProcessing::~FrameProcessing()
{
    // set up thread barrier before this object is destroyed
    std::for_each(_threads.begin(), _threads.end(), [](std::thread &t) {
        t.join();
    });
}

void FrameProcessing::setSagemakerHandle(std::shared_ptr<Sagemaker> sagemaker){
    _sagemaker = sagemaker;
}

FrameProcessing::FrameProcessing(){
    //create a pointer to processing objejct
    _sagemaker = std::make_shared<Sagemaker>();
    _sagemakerCallCount = 0;
}

void FrameProcessing::startProcessing(){
    // launch frame queue processing in a thread
    _threads.emplace_back(std::thread(&FrameProcessing::processFrameQueue, this));
    //launch thread to wait for Sagemaker response
    _threads.emplace_back(std::thread(&FrameProcessing::waitSagemakerCall, this));
}

std::future<void> FrameProcessing::addFrameToQueue(std::shared_ptr<Frame> frame, std::future<void> &&future)
{
    //Does not need mutex only one producer!
    // add new frame to the end of the waiting line
    std::promise<void> promiseFrameProcessed;
    future = promiseFrameProcessed.get_future();
    _waitingFrames.pushBack(frame, std::move(promiseFrameProcessed));

    // wait until frame has been processed
    return std::move(future);
}

void FrameProcessing::processFrameQueue(){
    while (true) {
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if(_waitingFrames.getSize() > 0 && _sagemakerCallCount < _sagemakerCallLimit){
            //Get first of the queue
            std::promise<void> promiseFrame;
            std::shared_ptr<Frame> frame;
            _waitingFrames.processFirstInQueue(frame, promiseFrame);
            //launch async call to Sagemaker
            auto sageFuture = std::async(&Sagemaker::infer, _sagemaker, frame);
            //Pass future to a process that waits for Sagemaker response
            _sagemakerInfeDone.pushBack(std::move(sageFuture), std::move(promiseFrame));
            //Increase call count
            std::lock_guard lck(_mutexSageCount);
            _sagemakerCallCount += 1;
        }
    }
}

//call WaitingSageResponse::waitForFirstInQueue() continously
void FrameProcessing::waitSagemakerCall(){
    while (true) {
        //Sagemaker takes more time to respond
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        bool frameProcessed;
        _sagemakerInfeDone.waitForFirstInQueue(frameProcessed);
        if(frameProcessed && _sagemakerCallCount > 0){
            std::lock_guard lck(_mutexSageCount);
            _sagemakerCallCount -= 1;
        }
    }
}
