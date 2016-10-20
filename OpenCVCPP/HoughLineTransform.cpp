#include <stdlib.h>
#include <iostream>
#include <vector>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include "HoughLineTransform.h"
using namespace std;
using namespace cv;

int rho = 1, theta = 0;
int max_rho = 100, max_theta = 180;
int hthreshold = 100;
int max_threshold = 200;
Mat g_src;
Mat g_dst;

void erosion(const Mat& src, Mat& dest)
{
	int erosion_type = 0;
	int erosion_elem = 0, erosion_size = 2;

	if (erosion_elem == 0) { erosion_type = MORPH_RECT; }
	else if (erosion_elem == 1) { erosion_type = MORPH_CROSS; }
	else if (erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

	Mat element = getStructuringElement(erosion_type,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));
	erode(src, dest, element);
}

void dilation(const Mat& src, Mat& dest)
{
	int dilation_type = 0;
	int dilation_elem = 0, dilation_size = 2;
	if (dilation_elem == 0) { dilation_type = MORPH_RECT; }
	else if (dilation_elem == 1) { dilation_type = MORPH_CROSS; }
	else if (dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }
	auto morph_type = MorphTypes(1);

	Mat element = getStructuringElement(dilation_type,
		Size(2 * dilation_size + 1, 2 * dilation_size + 1),
		Point(dilation_size, dilation_size));
	dilate(src, dest, element);

}

void RunHoughLine(int, void*);
static Mat src;
static Mat dst, cdst;
static Point2f points[4];
static int point_counter = 0;

vector<Point2f> point_list;

void GeoTransform()
{
	Point2f targets[] = { cvPoint2D32f(0,0), cvPoint2D32f(300,0), cvPoint2D32f(300,300), cvPoint2D32f(0,300) };
	Point2f src_points[4];
	for (int i = 0; i < 4; i++)
		src_points[i] = point_list[i];
	auto T = getPerspectiveTransform(src_points, targets);
	auto T2 = getAffineTransform(src_points, targets);
	Mat dest_image;
	//warpPerspective(src, dest_image, T, Size(300, 300));
	warpAffine(src, dest_image, T2, Size(300, 300));
	imshow("Transformed image", dest_image);
}

void Callback_Func(int _event, int x, int y, int flags, void *userdata) {
	printf("Handle event\n");
	if (_event == EVENT_LBUTTONDOWN)
	{
		cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
		Point pts(x, y);

		point_list.push_back(pts);
		if (point_list.size() > 4) {
			destroyWindow("Point picker");
			GeoTransform();
		}

	}
	else if (_event == EVENT_RBUTTONDOWN)
	{
		//cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (_event == EVENT_MBUTTONDOWN)
	{
		//cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (_event == EVENT_MOUSEMOVE)
	{
		//cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;
	}
}

void DemoLineTransform()
{
	const char filename[] = "./res/suduku.jpg";
	src = imread(filename, IMREAD_GRAYSCALE);
	if (src.empty())
	{
		cout << "Can not open file " << filename << endl;
		return;
	}

	GaussianBlur(src, src, Size(5, 5), 0);
	Canny(src, dst, 50, 200, 3);
	
	//erosion(dst, dst);
	dilation(dst, dst);
	erosion(dst, dst);
	imshow("Canny :", dst);
	cvtColor(dst, cdst, CV_GRAY2BGR);
#if 0
	vector<Vec2f> lines;
	HoughLines(dst, lines, 1, CV_PI / 180, 100, 0, 0);
	
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		line(cdst, pt1, pt2, Scalar(0, 0, 255), 3, CV_AA);
	}
#endif
	namedWindow("Point picker", CV_WINDOW_AUTOSIZE);
	imshow("Point picker", src);
	cvSetMouseCallback("Point picker", Callback_Func);

	namedWindow("Dest image", WINDOW_AUTOSIZE);
	

	RunHoughLine(0, NULL);
	createTrackbar("Control pixel width", "Dest image", &rho, max_rho, RunHoughLine);
	createTrackbar("Control pixel width 2", "Dest image", &theta, max_theta, RunHoughLine);
	createTrackbar("Threshold", "Dest image", &hthreshold, max_threshold, RunHoughLine);
	
	waitKey(0);
}

void RunHoughLine(int, void*)
{
	Mat show(dst.size(), dst.type());
#if 1
	vector<Vec4i> lines;
	//Mat dest(src.size(), g_src.type());
	
	if (src.empty()) return;
	HoughLinesP(dst, lines, (rho + 1), (CV_PI / 180) * (theta + 1), hthreshold + 1, 100);
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(show, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 1, CV_AA);
	}
#else
	vector<Vec2f> lines;
	HoughLines(dst, lines, (rho + 1) , CV_PI / (180 + theta) , hthreshold, 0, 0);

	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		line(show, pt1, pt2, Scalar(0, 0, 255), 3, CV_AA);
	}
#endif
	Mat sized;
	resize(show, sized, show.size() / 2);
	imshow("Dest image", sized);
	//cvSetMouseCallback("Dest image", Callback_Func, NULL);
}