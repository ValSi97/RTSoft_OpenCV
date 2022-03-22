// task1_hist.cpp: определяет точку входа для консольного приложения.
//
#include <iostream>
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

FileStorage fs;
string filename = "histograms/histogram.yml";
double compare_hists_arr[2][15];
int number;

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

// функция записи гистограмм в файл
void write_hist_to_file(Mat hist,int i)
{
	// save file

	string hist_name = "hist_" + to_string(i);
	fs << hist_name << hist;
}
//функция чтения гистограмм из файла
Mat read_hist_from_file(int i)
{
	// load hist from file
	Mat hist;
	fs.open(filename, FileStorage::READ);
	string hist_name = "hist_" + to_string(i);
	fs[hist_name] >> hist;
	fs.release();
	return hist;

}
//функция вывода массива на экран
void arr_output_to_screen(double arr[2][15])
{
	for (int i = 0; i<2; i++) {
		for (int j = 0; j<15; j++)
			cout << round(arr[i][j] * 100) / 100 << "\t";
		cout << endl;
	}
}

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
	public:
		sort_class(int nnumb) {
			numb = nnumb;
		}
		bool operator() (Img i, Img j)
	{

		return compare_images(i,j,numb);
	}
} ;



int main(int, char**) {
	fs.open(filename, FileStorage::WRITE);

	for (int i = 1; i <= 15; i++) {
		Mat gray;
		string image_name1 = "images/" + to_string(i);
		string image_name = image_name1 + ".jpg";
		Mat image = imread(image_name);

		cvtColor(image, gray, COLOR_BGR2GRAY); // Перевод в градации серого

		int histSize = 256; // bin size
		float range[] = { 0, 255 };
		const float *ranges[] = { range };

		Mat hist;
		calcHist(&gray, 1, 0, Mat(), hist, 1, &histSize, ranges, true, false); // Calculate histogram
		write_hist_to_file(hist, i);//Write histograms to file function

		Img image_obj(image_name, hist); //Создаем объект класса Image
		vec.push_back(image_obj); //Записываем его в вектор
		
		cout << "\nHistogramm of image with name: "<< i <<".jpg\n"<<endl;
		for (int h = 0; h < histSize; h++) { // Show the calculated histogram in command window
			float binVal = hist.at<float>(h);
			cout << " " << binVal;
		}

		cout << '\n';

	}
	fs.release();

	// ввод номера картинки для сравнения схожести и сортировки относительно ее
	cout << "\nInput number of image (1 to 15), which will be compare" << endl;
	cin >> number;

	read_hist_from_file(number);// прочитать гистограмму из файла

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