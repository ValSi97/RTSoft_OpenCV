// task1_hist.cpp: определяет точку входа для консольного приложения.
//
#include <iostream>
#include "stdafx.h"
#include <algorithm>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <E:\opencv\build\include\opencv2\xfeatures2d\include\opencv2/xfeatures2d.hpp>

using namespace std;
using namespace cv;

int number;
Ptr<Feature2D> f2d = xfeatures2d::

Mat createDescriptors(const int nnumb) {
	Mat descriptors, img;
	string image_name1 = "images/" + to_string(nnumb);
	string image_name = image_name1 + ".jpg";
	img = imread(image_name);
	vector<KeyPoint> keypoints;
	f2d->detectAndCompute(img, noArray(), keypoints, descriptors);
	return descriptors;
}

class Img {
	string name;
	Mat hist;
public:
	Img(string nname, Mat hhist);
	Img();
	~Img();
	string get_name();
	Mat get_hist();
};
Img::Img(string nname, Mat hhist)
{

	name = nname;
	hist = hhist;

}
Img::Img()
{
	name = " ";
	hist = 0;
}

Img::~Img()
{

}
Mat Img::get_hist()
{
	return hist;
}

string Img::get_name()
{
	return name;
}

vector<Img> vec;

void write_sorted_images()
{
	for (int i = 0; i < 15; i++) {
		string image_name = vec[i].get_name();
		Mat image = imread(image_name);

		string image_name2 = "sorted_images/" + to_string(i);
		image_name = image_name2 + ".jpg";

		vector<int> compression_params;
		compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
		compression_params.push_back(100);

		imwrite(image_name, image, compression_params);
	}
}

bool compare_images(Img i, Img j, int numb)
{
	Mat h1 = read_hist_from_file(numb);
	int index = 0;
	double compare_res1 = compareHist(h1, i.get_hist(), CV_COMP_CORREL);
	double compare_res2 = compareHist(h1, j.get_hist(), CV_COMP_CORREL);
	if (compare_res1 > compare_res2)
		return 1;
	else
		return 0;
}


class sort_class
{
	int numb;
	Mat cmpDescriptors;
public:
	sort_class(int nnumb) {

		numb = nnumb;
		cmpDescriptors = createDescriptors(cmpImage);
	}
	bool operator() (Img i, Img j)
	{

		return compare_images(i, j, numb);
	}
};



int main(int, char**) {

	for (int i = 1; i <= 15; i++) {
		Mat gray;
		string image_name1 = "images/" + to_string(i);
		string image_name = image_name1 + ".jpg";
		Mat image = imread(image_name);
		vec.push_back(image); //Записываем картинки в вектор
		cout << '\n';
	}


	// ввод номера картинки для сравнения схожести и сортировки относительно ее
	cout << "\nInput number of image (1 to 15), which will be compare" << endl;
	cin >> number;

	sort_class sc(number);
	sort(vec.begin(), vec.end(), sc);

	cout << "Sorting images with functor\n" << endl;
	for (int i = 0; i < vec.size(); ++i)
	{
		cout << "Image:\t" << vec[i].get_name() << "\tSimilarity:\t" << round(compareHist(vec[0].get_hist(), vec[i].get_hist(), CV_COMP_CORREL) * 100) / 100 << endl;
	}
	cout << endl;
	write_sorted_images(); //запись в порядке уменьшения схожести сортированных картинок относительно выбранной в отдельную папку sorted_images

	cv::waitKey(5000);
	system("pause");
}