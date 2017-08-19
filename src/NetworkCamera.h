#ifndef __NETWORK_CAMERA_H__
#define __NETWORK_CAMERA_H__

#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/core/core.hpp>  
using namespace cv;

#include "NetworkCameraStruct.h"

class NETWORKCAMERA_API  NetworkCamera
{
public:
	 NetworkCamera(const struct camera_info &c);
public:
	 bool isPlaying();
	 bool queryframe(cv::Mat &im);
private:
	void *impl_;
};

#endif