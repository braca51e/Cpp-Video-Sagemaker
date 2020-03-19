#ifndef FRAMEPROCESSING_H
#define FRAMEPROCESSING_H

#include <vector>
#include <future>
#include <mutex>
#include <memory>
#include <algorithm>
#include "Frame.h"
#include "Sagemaker.h"

// forward declarations to avoid include cycle
class Frame;
struct InfereceResult;

// auxiliary class to queue and dequeue waiting frames in a thread-safe manner
class WaitingFrames
{
public:
    // getters / setters
    int getSize();

    // typical behaviour methods
    void pushBack(std::shared_ptr<Frame> frame, std::promise<void> &&promise);
    void processFirstInQueue(std::shared_ptr<Frame> &frame, std::promise<void> &promise);

private:
    std::vector<std::shared_ptr<Frame>> _frames;          // frames waiting to enter be processed
    std::vector<std::promise<void>> _promises; // list of associated promises
    std::mutex _mutex;
};

class WaitingSageResponse
{
public:
    // getters / setters

    // typical behaviour methods
    void pushBack(std::future<void> &&future, std::promise<void> &&framePromise);
    void waitForFirstInQueue(bool &frameProcessed);

private:
    std::vector<std::future<void>> _futures; // list of associated promises
    std::vector<std::promise<void>> _framePromises; //list of frame promises
    std::mutex _mutexSage;
};

class FrameProcessing
{
public:
    // constructor / desctructor
    FrameProcessing();
    ~FrameProcessing();

    // getters / setters
    void setSagemakerHandle(std::shared_ptr<Sagemaker> sagemaker);

    // typical behaviour methods
    void startProcessing();
    std::future<void> addFrameToQueue(std::shared_ptr<Frame> frame, std::future<void> &&future);

private:
    //control number of calls to endpoint
    static const unsigned int _sagemakerCallLimit;
    static unsigned int _sagemakerCallCount;
    std::mutex _mutexSageCount;
    // typical behaviour methods
    void processFrameQueue();
    void waitSagemakerCall();
    // private members
    std::vector<std::thread> _threads; //thread that processes sagemaker infer done
    std::shared_ptr<Sagemaker> _sagemaker; // Handle to request entry to queue
    WaitingSageResponse _sagemakerInfeDone; // futures waiting for Sagemaker to process frame
    WaitingFrames _waitingFrames; // list of all frames and their associated promises waiting to be processed by sagemaker
};


#endif
