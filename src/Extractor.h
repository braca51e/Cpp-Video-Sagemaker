#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include <string>
#include <vector>
#include <thread>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "Frame.h"
#include "FrameProcessing.h"

// auxiliary class to queue and dequeue waiting frames in a thread-safe manner
class WaitingFrameFutures
{
public:
    // getters / setters

    // typical behaviour methods
    void pushBack(std::pair<std::shared_ptr<Frame>, std::future<void>> &&pair);
    void processFirstInQueue();

private:
    std::vector<std::pair<std::shared_ptr<Frame>, std::future<void>>> _frameFutures;
    std::mutex _mutexFrameFutures;
};

class Extractor
{
public:
    // constructor / desctructor
    Extractor();

    // getters / setters
    void setVideoName(std::string videoName);

    // typical behaviour methods
    void extractVideo();
    void loadVideo();

private:
    // typical behaviour methods
    void processFutures();

    // member variables
    bool _processingFrameFutures;
    std::string _videoName;
    cv::VideoCapture _cap;
    std::vector<std::thread> _threads;
    WaitingFrameFutures _waitingFrameFutures;
};

#endif
