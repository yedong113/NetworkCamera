// NetworkCamera.cpp : Defines the exported functions for the DLL application.
//


#include "NetworkCameraImplement.h"



NetworkCameraImplement::NetworkCameraImplement(const struct camera_info &c)
{
	const char * const vlcArgs[] = {
		"--demux=h264",
		"--ipv4",
		"--rtsp-caching=300",
		"--rtsp-tcp",
		// 		    "--extraintf=logger", // Log anything
		// 		    "--verbose=2", // Be much more verbose then normal for debugging purpose
	};
	cameraInfo.camera_id = c.camera_id;
	cameraInfo.camera_name = c.camera_name;
	cameraInfo.rtsp = c.rtsp;
	capture_speed = c.capture_speed;
	cameraInfo.capture_mode = c.capture_mode;
	cameraInfo.video_heigth = c.video_heigth;
	cameraInfo.video_width = c.video_width;

	pcamera = new K_CAMERA;
	pcamera->pContext = new K_CTX;
	pcamera->camera_id = cameraInfo.camera_id;
	pcamera->camera_name = cameraInfo.camera_name;
	pcamera->rtsp = cameraInfo.rtsp;
#ifdef _WIN32
    pcamera->pContext->mutex = ::CreateMutex(NULL, FALSE, NULL);
#else
    pcamera->pContext->mutex = PTHREAD_MUTEX_INITIALIZER;
#endif
	pcamera->pContext->image = new cv::Mat(cameraInfo.video_heigth, cameraInfo.video_width, CV_8UC3);
	pcamera->pContext->pixels = (unsigned char*)pcamera->pContext->image->data;
	pcamera->pContext->camera_id = pcamera->camera_id;
	pcamera->pContext->camera_name = pcamera->camera_name;


	thread_capture = NULL;
	thread_playing = NULL;
	initInstance(vlcArgs);
	thread_detect_playing = threadGroup.create_thread(boost::bind(&NetworkCameraImplement::procDetectPlaying, this));
}
bool NetworkCameraImplement::initInstance(const char *const * vlc_args)
{
	instance = VLC::Instance(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
	instance.setExitHandler([this] {
	});
	instance.logSet([this](int lvl, const libvlc_log_t*, std::string message) {
	});
	return true;
}


int  NetworkCameraImplement::playMedia()
{
	auto media = VLC::Media(instance, pcamera->rtsp, VLC::Media::FromLocation);
// 	media.addOption(":network-caching=300");
	auto mp = VLC::MediaPlayer(media);
	auto eventManager = mp.eventManager();
	eventManager.onPlaying([this, &media]() {
		mediaPlaying = true;
	});
	auto imgBuffer = pcamera;
	mp.setVideoCallbacks([imgBuffer](void** pBuffer) -> void* {

		PKCAMERA pcamera_ = (PKCAMERA)imgBuffer;
#ifdef __WIN32
        WaitForSingleObject(pcamera_->pContext->mutex, INFINITE);
#else
        pthread_mutex_lock(&pcamera_->pContext->mutex);
#endif
		*pBuffer = pcamera_->pContext->pixels;
		return NULL;
	}, [imgBuffer](void*, void*const*) {
		PKCAMERA pcamera_ = (PKCAMERA)imgBuffer;
#ifdef _WIN32
         ReleaseMutex(pcamera_->pContext->mutex);
#else
        pthread_mutex_unlock(&pcamera_->pContext->mutex);
#endif
		// 			std::cout << "unlock" << std::endl;
	}, nullptr
		);
	auto video_width = cameraInfo.video_width;
	auto video_heigth = cameraInfo.video_heigth;
	mp.setVideoFormatCallbacks([video_width, video_heigth](char* chroma, uint32_t* width, uint32_t* height, uint32_t* pitch, uint32_t* lines) -> int {
		memcpy(chroma, "RV24", 4);
		*width = video_width;
		*height = video_heigth;
		*pitch = *width * 24 / 8;
		*lines = 320;
		return 1;
	}, nullptr);
    mp.play();

	bool expected = true;

	mp.setAudioCallbacks([](const void*, uint32_t count, int64_t pts) {
	}, nullptr, nullptr, nullptr, nullptr
		);

	auto handler = mp.eventManager().onPositionChanged([&expected](float pos) {
		assert(expected);
	});
	std::this_thread::sleep_for(std::chrono::seconds(2));
	handler->unregister();
	expected = false;
	std::this_thread::sleep_for(std::chrono::seconds(2));
	expected = true;
	auto l = [&expected](float){
		assert(expected);
	};
	auto lFunc = std::function<void(float)>{ l };
	auto h1 = mp.eventManager().onTimeChanged(lFunc);
	auto h2 = mp.eventManager().onPositionChanged(lFunc);
	std::this_thread::sleep_for(std::chrono::seconds(2));
	mediaPlaying = mp.isPlaying();
	if (mediaPlaying)
	{
	}
	{
		LOCK lock(mutex);
		condition_variable.wait(lock);
	}
	mp.eventManager().unregister(h1, h2);
	expected = false;

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	// Using scopped event manager to automatically unregister events
	{
		expected = true;
		// This is a copy. Assigning to a reference wouldn't clear the registered events
		auto em = mp.eventManager();
		em.onPositionChanged([&expected](float) {
			assert(expected);
		});
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	expected = false;

	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	auto mp2 = mp;
	expected = true;
	auto h3 = mp2.eventManager().onStopped([&expected]() {
		std::cout << "MediaPlayer stopped" << std::endl;
		assert(expected);
		expected = false;
	});
	mp.stop();
	mp.eventManager().unregister(h3);

	auto audioFilters = instance.audioFilterList();
	for (const auto& f : audioFilters)
	{
		std::cout << f.name() << std::endl;
	}
	return 1;
}

bool NetworkCameraImplement::isPlaying()
{
	return mediaPlaying;
}

int  NetworkCameraImplement::procDetectPlaying()
{
	while (true)
	{
		if (!mediaPlaying || (mediaPlaying&&playingTimerOut()))
		{
			if (thread_playing != NULL)
			{
				condition_variable.notify_all();
				thread_playing->join();
			}
			thread_playing = threadGroup.create_thread(boost::bind(&NetworkCameraImplement::playMedia, this));
			start_time = ptime(second_clock::local_time());
		}
		boost::this_thread::sleep(boost::posix_time::seconds(40));
	}
}

//�ж���Ƶ����ʱ���Ƿ񳬹�ָ����ֵ
bool  NetworkCameraImplement::playingTimerOut()
{
	ptime curr_time = ptime(second_clock::local_time());
	time_duration dt = curr_time - start_time;
	int  t_seconds = dt.total_seconds();
	if (t_seconds < 3600)
	{
		return false;
	}
	return true;
}

bool NetworkCameraImplement::queryframe(cv::Mat &im)
{
	im = pcamera->pContext->image->clone();
	return true;
}