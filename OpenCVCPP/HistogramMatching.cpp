#include <opencv/cv.hpp>
#include <opencv/highgui.h>
#include <iostream>
#include "HistogramMatch.h"
using namespace cv;
using namespace std;

void display_two_image(const Mat& img1,const Mat& img2);


std::string window_name = "Hello";
#define DELAY_CAPTION 500
#define MAX_KERNEL_LENGTH 15
#define DISPLAY_DST(delay) if(display_dst(delay) != 0) { return 0 ; }
#define DISPLAY_CAP(caption) if (display_caption(caption) != 0) { return 0; }
#define MAX(a,b) (a > b) ? a : b
Mat src, dst;

void display_two_image(const Mat& img1, const Mat& img2) {
	Mat out(Size(img1.size().width + img2.size().width, MAX(img1.size().height, img2.size().height)), CV_8UC3);
	img1.copyTo(out(Rect(0, 0, img1.size().width, img1.size().height)));
	img2.copyTo(out(Rect(img1.size().width, 0, img2.size().width, img2.size().height)));
	imshow("DEMO", out);
	//waitKey(0);
}


int test_blur()
{
	const string filename = "./res/demo.jpg";
	const string demo1_path = "./res/demo1.jpg";
	Mat demo1 = imread(demo1_path, IMREAD_COLOR);
	src = imread(filename, IMREAD_COLOR);
	display_two_image(src, demo1);

	if (display_caption("Original Image") != 0) { return 0; }
	dst = src.clone();

	if (display_dst(DELAY_CAPTION) != 0) return 0;

	if (display_caption("Homogeneous Blur") != 0) return 0;


	for (int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2)
	{
		blur(src, dst, Size(i, i), Point(-1, -1));
		DISPLAY_DST(DELAY_CAPTION);
	}
	// Gaussian blur
	DISPLAY_CAP("GAUSSIAN BLUR")
	for (int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2)
	{
		GaussianBlur(src, dst, Size(i, i), 0, 0);
		DISPLAY_DST(DELAY_CAPTION);
	}

	DISPLAY_CAP("MEAN BLUR");
	for (int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2)
	{
		medianBlur(src, dst, i);
		DISPLAY_DST(DELAY_CAPTION);
	}

	DISPLAY_CAP("BILATERAL BLUR");
	for (int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2)
	{
		bilateralFilter(src, dst, i, i * 2, i / 2);
		DISPLAY_DST(DELAY_CAPTION);
	}

	

	waitKey(0);
	return 0;
}

int calc_HueSaturation(char* filename)
{
	Mat src, hsv;
	if (!(src = imread(filename, 1)).data)
		return -1;

	cvtColor(src, hsv, CV_BGR2HSV);

	// Quantize the hue to 30 levels
	// and the saturation to 32 levels
	int hbins = 30, sbins = 32;
	int histSize[] = { hbins, sbins };
	// hue varies from 0 to 179, see cvtColor
	float hranges[] = { 0, 180 };
	// saturation varies from 0 (black-gray-white) to
	// 255 (pure spectrum color)
	float sranges[] = { 0, 256 };
	const float* ranges[] = { hranges, sranges };
	MatND hist;
	// we compute the histogram from the 0-th and 1-st channels
	int channels[] = { 0, 1 };

	calcHist(&hsv, 1, channels, Mat(), // do not use mask
		hist, 2, histSize, ranges,
		true, // the histogram is uniform
		false);
	double maxVal = 0;
	minMaxLoc(hist, 0, &maxVal, 0, 0);

	int scale = 10;
	Mat histImg = Mat::zeros(sbins*scale, hbins * 10, CV_8UC3);

	for (int h = 0; h < hbins; h++)
		for (int s = 0; s < sbins; s++)
		{
			float binVal = hist.at<float>(h, s);
			int intensity = cvRound(binVal * 255 / maxVal);
			rectangle(histImg, Point(h*scale, s*scale),
				Point((h + 1)*scale - 1, (s + 1)*scale - 1),
				Scalar::all(intensity),
				CV_FILLED);
		}

	namedWindow("Source", 1);
	imshow("Source", src);

	namedWindow("H-S Histogram", 1);
	imshow("H-S Histogram", histImg);
	waitKey();
}

int display_caption(const char* caption)
{
	dst = Mat::zeros(src.size(), src.type());
	putText(dst, caption,
		Point(src.cols / 4, src.rows / 2),
		FONT_HERSHEY_COMPLEX, 0.5, Scalar(255, 255, 0));
	imshow(window_name, dst);
	int c = waitKey(DELAY_CAPTION);
	if (c >= 0) { return -1; }
	return 0;
}

int display_dst(int delay)
{
	imshow(window_name, dst);
	int c = waitKey(delay);
	if (c >= 0) return -1;
	return 0;
}

void erosion(int, void*);
void dilation(int, void*);

int erosion_elem = 0;
int dilation_elem = 0;
int max_elem = 2;
int erosion_size = 1, max_size = 21;
int dilation_size = 1;
Mat erosion_dst, dilation_dst;

void demo_erosion()
{
	// Load image
	src = imread("./res/demo1.jpg", IMREAD_COLOR);
	
	if (!src.data) return;
	namedWindow("Erosion Demo", WINDOW_AUTOSIZE);
	namedWindow("Dilation Demo", WINDOW_AUTOSIZE);
	//moveWindow("Dilation Demo", src.cols, 0);

	createTrackbar("Element:\n 0: Rect \n 1: Cross \n 2: Ellipse", "Erosion Demo", &erosion_elem, max_elem, erosion);
	createTrackbar("Kernel size: ", "Erosion Demo", &erosion_size, max_size, erosion);

	createTrackbar("Element:\n 0: Rect \n 1: Cross \n 2: Ellipse", "Dilation Demo", &dilation_elem, max_elem, dilation);
	createTrackbar("Kernel size: ", "Dilation Demo", &dilation_size, max_size, dilation);

	for (int i = 0; i < 8; i++) {
		auto morph_type = MorphShapes(i);
		
		Mat element = getStructuringElement(morph_type, Size(7, 7), Point(3, 3));
		cout << "Morph type " << i << " " << " type name" << endl;
		cout << element << endl;
		
	}

	erosion(0, 0);
	dilation(0, 0);
	waitKey(0);
}

void erosion(int, void*)
{
	int erosion_type = 0;
	if (erosion_elem == 0) { erosion_type = MORPH_RECT; }
	else if (erosion_elem == 1) { erosion_type = MORPH_CROSS;  }
	else if (erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

	Mat element = getStructuringElement(erosion_type,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));
	erode(src, erosion_dst, element);
	imshow("Erosion Demo", erosion_dst);
}

void dilation(int, void*)
{
	int dilation_type = 0;
	if (dilation_elem == 0) { dilation_type = MORPH_RECT;  }
	else if (dilation_elem == 1) { dilation_type = MORPH_CROSS; }
	else if (dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }
	auto morph_type = MorphTypes(1);

	Mat element = getStructuringElement(dilation_type,
		Size(2 * dilation_size + 1, 2 * dilation_size + 1),
		Point(dilation_size, dilation_size));
	dilate(src, dilation_dst, element);
	imshow("Dilation Demo", dilation_dst);
	
}