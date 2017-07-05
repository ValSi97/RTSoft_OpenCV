#include "stdafx.h"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"


using namespace std;
using namespace cv;


int main(int argc, const char** argv)

{

	namedWindow("test_window", CV_WINDOW_AUTOSIZE);
	char* filename = "video.MOV";
	CvCapture* capture = cvCreateFileCapture(filename);
	//для записи считанных кадров и измененных в отдельное видео
	const char *modified_video = "result.avi";
	// частота кадров
	//double fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
	CvSize size = cvSize(640, 480);
	CvVideoWriter *writer = cvCreateVideoWriter(modified_video, CV_FOURCC('X', 'V', 'I', 'D'), 10, size, 1);
	assert(writer != 0);
	
	while (1) {

		IplImage* frame = 0;
		IplImage* gray = 0;
		IplImage* dst_canny = 0;
		IplImage* gray_contrast = 0;
		//IplImage* gray_blur = 0;

		frame = cvQueryFrame(capture); // сделали кадр из видео


		if (!frame) {

			printf("Cannot load imagine\n");
			break;
		}


		CvMemStorage* storage = cvCreateMemStorage(0);
		CvSeq* lines = 0;
		int i = 0;

		gray = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
		dst_canny = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
		gray_contrast = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
		//gray_blur = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);

		Mat gray_blur_mat;
		Mat gray_contrast_mat = cvarrToMat(gray_contrast);

		cvCvtColor(frame, gray, CV_RGB2GRAY);
		cvEqualizeHist(gray, gray_contrast); //увеличение контраста

		GaussianBlur(gray_contrast_mat, gray_blur_mat, Size(5, 5), 0, 0); // добавление блюра для уменьшения помех

		IplImage* gray_final = cvCloneImage(&(IplImage)gray_blur_mat);

		cvCanny(gray_final, dst_canny, 180, 10, 3);

		lines = cvHoughLines2(dst_canny, storage, 1, 1, CV_PI / 180, 20, 8, 3);
		//cvShowImage("canny", dst_canny);
		for (int i = 0; i < lines->total; i++) {

			CvPoint* line = (CvPoint*)cvGetSeqElem(lines, i);

			cvLine(frame, line[0], line[1], CV_RGB(0, 255, 0), 1);


		}
		//покадровая запись в видео
		//int res = cvWriteFrame(writer, frame);

		//cvShowImage("original", writer);
		cvShowImage("original", gray);
		cvShowImage("blur_and_contrast", gray_final);
		cvShowImage("test_window", frame);


		if (waitKey(30) == 27) {

			break;
		}

	}
	// освобождаем ресурсы
	cvReleaseCapture(&capture);
	cvReleaseVideoWriter(&writer);
	cvDestroyWindow("original");
	cvDestroyWindow("blur_and_contrast");
	cvDestroyWindow("original");
	return 0;
}



