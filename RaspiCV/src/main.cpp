#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <stdint.h>
#include <bits/stdc++.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

/* Logger */
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

/* RPi camera mmap wrapper */
#include <opencv.hpp>
#include <raspicam/raspicam_cv.h>

using namespace std;
using namespace chrono;
using namespace cv;
using namespace raspicam;

/* Enable GUI interface */
#define GUI_SUPPORT
/* Window name */
#define GUI_WINDOW_NAME "Live Stream"
/* Maximum FPS */
#define MAX_FPS         28
/* Define frames dimensions */
#define FRAME_WIDTH     640
#define FRAME_HEIGHT    480
/* Camera1 device minor /dev/videoX */
#define DEVICE_CAMERA_MINOR 0

/* Break the loop when false */
bool _ProgramContinue = true;

/* Logger instance - multi-threaded */
auto console = spdlog::stdout_color_mt("Console");

void SigHandler(int signum)
{
	std::cout << std::endl;
	console->critical("Interrupt signal ({0} - {1}) received", strsignal(signum), signum);
	_ProgramContinue = false;
	
	exit(1);
}

int main(int argc, char **argv)
{
	/* Handle interruption signals */
	signal(SIGINT, SigHandler);
	signal(SIGQUIT, SigHandler);
	signal(SIGTSTP, SigHandler);
	
	/* Set console to display logs */
	console->set_level(spdlog::level::info);
	
	/* Camera handled */
	RaspiCam_Cv cam;
	if( !cam.set(cv::CAP_PROP_FPS, 0) ); //console->warn("Failed to set {0} fps", MAX_FPS);
	if( !cam.set(cv::CAP_PROP_FRAME_WIDTH, FRAME_WIDTH) ) console->warn("Failed to set camera width: {0}", FRAME_WIDTH);
	if( !cam.set(cv::CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT) ) console->warn("Failed to set camera height: {0}", FRAME_HEIGHT);
	if( !cam.set(CAP_PROP_FORMAT, CV_8UC1) ) console->warn("Failed to set prop format: {0}", CV_8UC1);
	
	if( !cam.open() )
	{
		console->warn("Error opening the camera");
		return 1;
	}
	
	/* Current read frame buffer */
	Mat FrameBuffer;
	
	/* Frames counter */
	static int fps = 0, last_fps = 0;
	static auto t1 = high_resolution_clock::now();
	static auto t2 = high_resolution_clock::now();
	
	while( true )
	{
		if( !cam.isOpened() )
		{
			cam.release();
			console->error("Camera not oppened!");
			return 1;
		}
		
		cam.grab();
		cam.retrieve(FrameBuffer);
		
		/* Print frames at console */
		fps++;
		t1 = high_resolution_clock::now();

#ifdef GUI_SUPPORT
		if( duration_cast<milliseconds>(t1 - t2).count() > 1000 )
		{
			t2 = t1;
			console->info("FPS: {0}", fps);
			last_fps = fps;
			fps = 0;
		}
		
		cv::putText(FrameBuffer, "FPS: " + to_string(last_fps), cv::Point(30, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8,
					cv::Scalar(100, 100, 150), 1, cv::LINE_AA);
		
		
		imshow(GUI_WINDOW_NAME, FrameBuffer);
		/* Necessary to display image */
		if( waitKey(30) == 27 )
			break;
#else
		if( duration_cast<milliseconds>(t1 - t2).count() > 1000 )
		{
			t2 = t1;
			console->info("FPS: {0}", fps);
			fps = 0;
		}
#endif
		
		
		/* Break if exit was triggered */
		if( !_ProgramContinue )
		{
			cam.release();
			break;
		}
	}
	
	return 0;
}



























