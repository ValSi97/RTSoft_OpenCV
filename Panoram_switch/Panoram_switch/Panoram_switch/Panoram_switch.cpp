

#include "stdafx.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/core/cuda.hpp"
#include "Panoram_switch.h"



int main()
{
	using namespace cv;
	using namespace cv::xfeatures2d;
	using namespace std;
	using namespace cv::cuda;
	VideoCapture cap_1("video_1.mp4"); // open the video file for reading
	VideoCapture cap_2("video_2.mp4"); // open the video file for reading

	Mat frame_1, frame_2;
	Mat gray_frame_1, gray_frame_2;
	Mat description_video1;
	Mat description_video2;

	if (!cap_1.isOpened() || !cap_2.isOpened()) return -1;

	while (1) {

		// read a new frame from video
		bool bSuccess_1 = cap_1.read(frame_1); 
		bool bSuccess_2 = cap_2.read(frame_2);

		if (!bSuccess_1 || !bSuccess_2)
		{
			std::cout << "Cannot read the frame from video files" << std::endl;
			break;
		}

		resize(frame_1, frame_1, Size(640, 480));
		resize(frame_2, frame_2, Size(640, 480));

		//Covert to Grayscale
		cvtColor(frame_1, gray_frame_1, COLOR_RGB2GRAY);
		cvtColor(frame_2, gray_frame_2, COLOR_RGB2GRAY);

		//--Step 1 : Detect the keypoints using SURF Detector
		int minHessian = 2000;
		Ptr <SurfFeatureDetector> detector = SurfFeatureDetector::create(minHessian);
		vector< KeyPoint > keypoints_object, keypoints_scence;

		detector->detect(gray_frame_1, keypoints_object);
		detector->detect(gray_frame_2, keypoints_scence);

		Ptr<SurfDescriptorExtractor> extractor = SurfDescriptorExtractor::create();

		extractor->compute(gray_frame_1, keypoints_object, description_video1);
		extractor->compute(gray_frame_1, keypoints_scence, description_video2);

		//Match image
		FlannBasedMatcher matcher;

		vector<DMatch> matches;

		matcher.match(description_video1, description_video2, matches);

		double minDist = 100;
		double maxDist = 0;

		for (int i = 0; i < description_video1.rows; i++) {

			double dist = matches[i].distance;
			if (dist > maxDist) maxDist = dist;
			if (dist < minDist) minDist = dist;
		}

		IplImage* fframe_1 = cvCloneImage(&(IplImage)gray_frame_1);
		IplImage* fframe_2 = cvCloneImage(&(IplImage)gray_frame_2);


		imshow("video1", gray_frame_1);
		imshow("video2", gray_frame_2);

		/*cvNamedWindow("video_1", 0);
		cvNamedWindow("video_2", 0);
		resizeWindow("video_1", 800, 600);
		resizeWindow("video_2", 640, 480);
		cvShowImage("video_1", fframe_1);
		cvShowImage("video_2", fframe_2);
		*/
		if (waitKey(30) == 27) {
			break;
		}
	}
    return 0;
}

