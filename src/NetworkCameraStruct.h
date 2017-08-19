#ifndef __NETWORK_CAMERA_STRUCT_HPP
#define __NETWORK_CAMERA_STRUCT_HPP

#include <string>


#ifdef _WIN32

#ifdef NETWORKCAMERA_EXPORT
#define NETWORKCAMERA_API __declspec(dllexport)
#else
#define NETWORKCAMERA_API __declspec(dllimport)
#endif
#else
#define NETWORKCAMERA_API __attribute__ ((visibility("default")))
#endif
typedef void* HANDLE;

#ifdef _WIN32
#else
#include <pthread.h>
#endif


typedef struct _K_CTX_
{
	int     camera_id;
	std::string       camera_name;
	void           * user_data;
#ifdef _WIN32
    HANDLE           mutex;
#else
    pthread_mutex_t  mutex;
#endif

	cv::Mat        * image;
	uchar          * pixels;
}K_CTX, *PKCTX;



struct camera_info
{
	camera_info() :camera_id(0), capture_speed(2){}
	int            camera_id;
	int            capture_speed;
	std::string    camera_name;
	int            capture_mode;
	std::string    rtsp;
	std::string    rtsp_arg;
	int    video_width;
	int    video_heigth;
};



typedef struct _K_CAMERA_
{
	int            camera_id;
	std::string    camera_name;
	std::string    rtsp;
	int            connected;
	PKCTX           pContext;
	int            capture_mode;
	std::string    rtsp_arg;
	int    video_width;
	int    video_heigth;
	//	ptime          start_time;
}K_CAMERA, *PKCAMERA;




#endif
