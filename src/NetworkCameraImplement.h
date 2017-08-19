#ifndef __NETWORK_CAMERA_IMPLEMENT_HPP_
#define __NETWORK_CAMERA_IMPLEMENT_HPP_

#include "vlcpp/vlc.hpp"
#include <iostream>
#include <thread>
#include <cstring>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/condition.hpp>
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/core/core.hpp>  
using namespace cv;
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <fstream>

using namespace boost::posix_time;

#include "NetworkCameraStruct.h"


class   NetworkCameraImplement
{
	typedef boost::mutex::scoped_lock LOCK;
public:
	PKCAMERA     pcamera;
	NETWORKCAMERA_API NetworkCameraImplement(const struct camera_info &c);
private:
	bool  initInstance(const char *const * vlc_args);
	int   playMedia();
	int   procDetectPlaying();
	bool  playingTimerOut();
public:
	NETWORKCAMERA_API bool isPlaying();
	NETWORKCAMERA_API bool queryframe(cv::Mat &im);
private:
	int index;
	int capture_speed;
	bool mediaPlaying;//��Ƶ���Ƿ��ڲ���
	VLC::Instance  instance;
	boost::thread_group  threadGroup;
	boost::thread      * thread_capture;
	boost::thread      * thread_playing;
	boost::thread      * thread_detect_playing;
	boost::mutex mutex;
	boost::condition condition_variable;
	ptime   start_time;//��Ƶ��ʼ����ʱ��
	struct camera_info cameraInfo;
};


#endif
