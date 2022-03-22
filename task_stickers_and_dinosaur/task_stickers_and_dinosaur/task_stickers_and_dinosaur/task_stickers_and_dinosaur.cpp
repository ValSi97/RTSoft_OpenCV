//����������� ��������� ��������� ����
// - ������ ����������� � ������ � ��������;
// - ������ ����������� �����������
// - ������� ������� ������ �� �����
// - ���������� ��������� ������������� � ����� �������� �������

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <math.h>

using namespace std;
using namespace cv;

// ������� ���������� � stickersCoords ���������� ��������

void recogniseStickersByThreshold(Mat image, vector<vector<Point>> &stickersCoords, vector<Vec4i> &hierarchy) {
	Mat image_hsv;
	cvtColor(image, image_hsv, COLOR_BGR2HSV); // ����������� � hsv
	Mat tmp_img(image.size(), CV_8U);
	// ��������� ���������� �� ����� ��������. ���� �������� ���������� :)
	inRange(image_hsv, Scalar(100, 100, 100), Scalar(255, 255, 255), tmp_img);
	// "��������" ������ � ��� ��������� �� �����
	dilate(tmp_img, tmp_img, Mat(), Point(-1, -1), 3);
	erode(tmp_img, tmp_img, Mat(), Point(-1, -1), 1);
	//��������� ����������� ��������

	findContours(tmp_img, stickersCoords,hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
}

bool cmpPoint(const Point &a, const Point &b) {
	return a.x < b.x;
}

int main() {
	cout << "start" << endl;
	VideoCapture cap("stickers.mov"); // open the video file for reading
	if (!cap.isOpened()) return -1;
	double fps = cap.get(CV_CAP_PROP_FPS); //get the frames per seconds of the video
	cout << "Frame per seconds : " << fps << endl;

	Mat dinosaur = imread("dinosaur.png");
	vector<vector<Point>> stickersCoords;
	vector<Vec4i> hierarchy;
	while (1) {
		Mat frame;
		bool bSuccess = cap.read(frame); // read a new frame from video
		if (!bSuccess) {
			cout << "Cannot read the frame from video file" << endl;
			break;
		}

		recogniseStickersByThreshold(frame, stickersCoords, hierarchy);
		Point sticker1 = *max_element(stickersCoords.front().begin(), stickersCoords.front().end(), cmpPoint);
		Point sticker2 = *min_element(stickersCoords.back().begin(), stickersCoords.back().end(), cmpPoint);
		// ���������������
		int side = abs(sticker1.x - sticker2.x);
		Mat imgForPaste;
		resize(dinosaur, imgForPaste, Size(side, side));
		// ������� ��������
		Mat roi = frame(Rect((min(sticker1.x, sticker2.x)), (min(sticker1.y, sticker2.y)) - side / 3, side, side));    // ���������� frame
		imgForPaste.copyTo(roi);
		//IplImage* iplimage_frame = cvCloneImage(&(IplImage)frame);
		cvShowImage("video", cvCloneImage(&(IplImage)frame));
		
		if (waitKey(30) == 27) {
			break;
		}
	}
	return 0;
}