# Concurrent Sagemaker Calls for Video Analytics

<!--- <img src="data/*.gif"/> --->

This project calls sagemaker endpoint concurrently for image detection. Only tested on macOS Mojave!

## Dependencies for Running Locally
* cmake >= 2.8
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac), 3.81 (Windows)
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
* gcc/g++ >= 5.4
  * Mac: same deal as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
* OpenCV >= 4.1
  * The OpenCV 4.1.0 source code can be found [here](https://github.com/opencv/opencv/tree/4.1.0)
* aws
  * aws-sdk-cpp: [Click here for installation instructions](https://github.com/aws/aws-sdk-cpp)

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory in the top level directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./VideoAnalysis --input /path/video.mp4`.
