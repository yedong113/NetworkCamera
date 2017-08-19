#include "NetworkCamera.h"
#include "NetworkCameraImplement.h"

NetworkCamera::NetworkCamera(const struct camera_info &c)
{
	NetworkCameraImplement *pNetworkCamera = new NetworkCameraImplement(c);
	impl_ = (void *)pNetworkCamera;
}

bool NetworkCamera::isPlaying()
{
	NetworkCameraImplement *pNetworkCamera = (NetworkCameraImplement *)impl_;
	return pNetworkCamera->isPlaying();
}


bool NetworkCamera::queryframe(cv::Mat &im)
{
	NetworkCameraImplement *pNetworkCamera = (NetworkCameraImplement *)impl_;
	return pNetworkCamera->queryframe(im);
}