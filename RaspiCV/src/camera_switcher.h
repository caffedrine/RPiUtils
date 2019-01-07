//
// Created by curiosul on 12/10/18.
//

#ifndef RPICAMERAMULTIPLEXER_CAMERA_SWITCHER_H
#define RPICAMERAMULTIPLEXER_CAMERA_SWITCHER_H

/* Logger */
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

/* GPIO */
#include "wiringPi/wiringPi.h"

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace chrono;
using namespace cv;

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

/* Number of cameras on the shield */
#define CAMS_NO  4

/* Cameras switch pins */
#define GPIO_CAM12_EN           21
#define GPIO_CAM34_EN           26
#define GPIO_GROUP_SELECT_14    24
#define GPIO_CAM_BUSY           7

#define GPIO_CAM56_EN           12
#define GPIO_CAM78_EN           19
#define GPIO_GROUP_SELECT_58    18

/* current camera used to capture */
int current_cam_no = 1;

/* Logger instance - multi-threaded */
auto console = spdlog::stdout_color_mt("Console");

/* Break the loop when false */
bool _ProgramContinue = true;

/* Video capture instance */
raspicam::RaspiCam_Cv cams[CAMS_NO];

/* Used to switch between cameras 1-4 */
static inline void select_camera(int camera)
{
	if( camera > CAMS_NO )
		camera = 1;
	else if( camera < 1 )
		camera = 4;
	
	switch( camera )
	{
		case 1:    /* Select camera 1 */
		{
			digitalWrite(GPIO_GROUP_SELECT_14, 0);
			digitalWrite(GPIO_CAM12_EN, 0);
			digitalWrite(GPIO_CAM34_EN, 1);
			current_cam_no = 1;
		}
			break;
		
		case 2: /* Select camera 2 */
		{
			digitalWrite(GPIO_GROUP_SELECT_14, 1);
			digitalWrite(GPIO_CAM12_EN, 0);
			digitalWrite(GPIO_CAM34_EN, 1);
			current_cam_no = 2;
		}
			break;
		
		case 3: /* Select camera 3 */
		{
			digitalWrite(GPIO_GROUP_SELECT_14, 0);
			digitalWrite(GPIO_CAM12_EN, 1);
			digitalWrite(GPIO_CAM34_EN, 0);
			current_cam_no = 3;
		}
			break;
		
		case 4: /* Select camera 4 */
		{
			digitalWrite(GPIO_GROUP_SELECT_14, 1);
			digitalWrite(GPIO_CAM12_EN, 1);
			digitalWrite(GPIO_CAM34_EN, 0);
			current_cam_no = 4;
			
		}
			break;
		
		default: /* Throw error if camera is invalid */
		{
			console->error("Invalid camera selected: {}", camera);
			exit(-1);
		}
	}
}

static inline void reset_cams()
{
	/* Write deaults */
	digitalWrite(GPIO_GROUP_SELECT_14, 0);
	digitalWrite(GPIO_GROUP_SELECT_58, 0);
	digitalWrite(GPIO_CAM12_EN, 1);
	digitalWrite(GPIO_CAM34_EN, 1);
	digitalWrite(GPIO_CAM56_EN, 1);
	digitalWrite(GPIO_CAM78_EN, 1);
}

int init()
{
	console->info("Initializing gpio...");
	/* WiringPi init */
	if( wiringPiSetupGpio() != 0 )
	{
		console->error("Failed to initialize GPIO pins!");
		return -1;
	}
	/* Config gpio pins as outputs*/
	pinMode(GPIO_CAM12_EN, OUTPUT);
	pinMode(GPIO_CAM34_EN, OUTPUT);
	pinMode(GPIO_GROUP_SELECT_14, OUTPUT);
	pinMode(GPIO_CAM_BUSY, INPUT);
	pinMode(GPIO_CAM56_EN, OUTPUT);
	pinMode(GPIO_CAM78_EN, OUTPUT);
	pinMode(GPIO_GROUP_SELECT_58, OUTPUT);
	
	/* Reset cams */
	reset_cams();
	
	/* Select first camera by default */
	select_camera(2);
	
	/* Wait a short period */
	this_thread::sleep_for(milliseconds(100));

//     while(1)
//     {
//     	select_camera(++current_cam_no);
//     	console->info("current camera: {0}", current_cam_no);
//
//     	cin.get();
//     }
	
	return 0;
}

/**
     * @brief makeCanvas Makes composite image from the given images
     * @param vecMat Vector of Images.
     * @param windowHeight The height of the new composite image to be formed.
     * @param nRows Number of rows of images. (Number of columns will be calculated
     *              depending on the value of total number of images).
     * @return new composite image.
     */
cv::Mat makeCanvas(std::vector<cv::Mat> &vecMat, int windowHeight, int nRows)
{
	/* Number of windows */
	int N = vecMat.size();
	/* Handle empty elements */
	for( int i = 0; i < N; i++ )
	{
		if( vecMat[i].empty() )   /* If the frame is empty */
		{
			Mat tmp(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC3, Scalar(0, 0, 0)); /* Just asign a random bg color */
			tmp.copyTo(vecMat[i]);
		}
	}
	
	nRows = nRows > N ? N : nRows;
	int edgeThickness = 10;
	int imagesPerRow = ceil(double(N) / nRows);
	int resizeHeight = floor(2.0 * ((floor(double(windowHeight - edgeThickness) / nRows)) / 2.0)) - edgeThickness;
	int maxRowLength = 0;
	
	std::vector<int> resizeWidth;
	for( int i = 0; i < N; )
	{
		int thisRowLen = 0;
		for( int k = 0; k < imagesPerRow; k++ )
		{
			double aspectRatio = double(vecMat[i].cols) / vecMat[i].rows;
			int temp = int(ceil(resizeHeight * aspectRatio));
			resizeWidth.push_back(temp);
			thisRowLen += temp;
			if( ++i == N ) break;
		}
		if( (thisRowLen + edgeThickness * (imagesPerRow + 1)) > maxRowLength )
		{
			maxRowLength = thisRowLen + edgeThickness * (imagesPerRow + 1);
		}
	}
	int windowWidth = maxRowLength;
	cv::Mat canvasImage(windowHeight, windowWidth, CV_8UC3, Scalar(0, 0, 0));
	
	for( int k = 0, i = 0; i < nRows; i++ )
	{
		int y = i * resizeHeight + (i + 1) * edgeThickness;
		int x_end = edgeThickness;
		for( int j = 0; j < imagesPerRow && k < N; k++, j++ )
		{
			int x = x_end;
			cv::Rect roi(x, y, resizeWidth[k], resizeHeight);
			cv::Size s = canvasImage(roi).size();
			// change the number of channels to three
			cv::Mat target_ROI(s, CV_8UC3);
			if( vecMat[k].channels() != canvasImage.channels() )
			{
				if( vecMat[k].channels() == 1 )
				{
					cv::cvtColor(vecMat[k], target_ROI, cv::COLOR_GRAY2BGR);
				}
			}
			else
			{
				vecMat[k].copyTo(target_ROI);
			}
			cv::resize(target_ROI, target_ROI, s);
			if( target_ROI.type() != canvasImage.type() )
			{
				target_ROI.convertTo(target_ROI, canvasImage.type());
			}
			target_ROI.copyTo(canvasImage(roi));
			x_end += resizeWidth[k] + edgeThickness;
		}
	}
	return canvasImage;
}

void print_fps()
{
	static int frames = 0;
	static auto t1 = high_resolution_clock::now();
	static auto t2 = high_resolution_clock::now();
	
	frames++;
	t1 = high_resolution_clock::now();
	if( duration_cast<milliseconds>(t1 - t2).count() > 1000 )
	{
		t2 = t1;
		console->info("FPS: {0}", frames);
		frames = 0;
	}
}

#endif //RPICAMERAMULTIPLEXER_CAMERA_SWITCHER_H
