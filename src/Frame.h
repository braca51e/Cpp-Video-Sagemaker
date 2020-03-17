#ifndef FRAME_H
#define FRAME_H

#include <string>
#include <iostream>
#include <opencv2/highgui.hpp>
#include "FrameProcessing.h"

// forward declarations to avoid include cycle
class FrameProcessing;

struct InfereceResult{
    public:
        float confidence;
        float bboxX;
        float bboxY;
        float bboxW;
        float bboxH;
        std::string label;
        int frameNumber;
};

class Frame: public std::enable_shared_from_this<Frame>
{
public:
    // constructor / desctructor
    Frame();
    // getters / setters
    int getFrameNumber(){return _frameNumber;};
    std::vector<uchar> getFrameBuffer(){return _frameBuffer;};
    void setInferenceResult(InfereceResult inferenceResult){_inferenceResult = inferenceResult;};
    void setFrameBuffer(cv::Mat &&frame);
    void setFrameProcessing(std::shared_ptr<FrameProcessing> frameProcessing){ _frameProcessing = frameProcessing;};

    // typical behaviour methods
     std::future<void> process();

    // miscellaneous
    std::shared_ptr<Frame> get_shared_this() { return shared_from_this(); }

private:
    // typical behaviour methods
    std::shared_ptr<FrameProcessing> _frameProcessing; // Handle to request entry to queue
    InfereceResult _inferenceResult;
    //encoded frame
    std::vector<uchar> _frameBuffer;
    //cv::Mat _frame;
    static int _frameCnt; // global variable for counting frames
    int _frameNumber; // every frame object has its own unique id
};

#endif
