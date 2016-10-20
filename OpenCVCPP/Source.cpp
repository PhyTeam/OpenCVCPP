#include "FourierTransform.h"
#include <stdlib.h>
#include <stdio.h>

#include <opencv2\opencv.hpp>
using namespace std;
using namespace cv;
	void test()
	{
		Mat img;
		// Draw a rectangle 
		cvRectangle(&img, cvPoint(0, 0), cvPoint(4, 4), cvScalar(255, 255, 255));
		// Draw a circle

	}

	float sum(CvMat* pMat)
	{
		float sum = 0.0f;
		for (size_t i = 0; i < pMat->rows; i++)
		{
			const float* begin_row = (float*)(pMat->data.ptr + i * pMat->step);
			for (size_t j = 0; j < pMat->cols; j++)
			{
				cout << CV_MAT_ELEM(*pMat, float, i, j) << " ";
				//sum += *(begin_row++);
			}
			cout << endl;
		}
		return sum;
	}

	Mat* LinearTransformation(float a, float b, const Mat* image)
	{
		Mat* output = new Mat(image->rows, image->cols, CV_32FC1);

		for (size_t i = 0; i < image->rows; i++)
		{
			const unsigned char* row_ptr = image->ptr<unsigned char>(i);
			float* ptr = output->ptr<float>(i);

			for (size_t j = 0; j < image->cols; j++)
			{

				ptr[j] = a * (row_ptr[j] / 256.0f) + b;
			}
		}
		return output;
	}
	void function(float, const int*) {

	}
	string type2str(int type) {
		string r;

		uchar depth = type & CV_MAT_DEPTH_MASK;
		uchar chans = 1 + (type >> CV_CN_SHIFT);

		switch (depth) {
		case CV_8U:  r = "8U"; break;
		case CV_8S:  r = "8S"; break;
		case CV_16U: r = "16U"; break;
		case CV_16S: r = "16S"; break;
		case CV_32S: r = "32S"; break;
		case CV_32F: r = "32F"; break;
		case CV_64F: r = "64F"; break;
		default:     r = "User"; break;
		}

		r += "C";
		r += (chans + '0');

		return r;
	}

	Mat* LogTranformation(float a, float b, const Mat* image)
	{
		cout << "Type " << type2str(image->type()) << endl;
		cout << "Size unsigned char: " << sizeof(unsigned char) << endl;
		cout << "Channels : " << image->channels() << endl;
		Mat* output = new Mat(image->rows, image->cols, CV_32FC1);

		for (size_t i = 0; i < image->rows; i++)
		{
			const unsigned char* row_ptr = image->ptr<unsigned char>(i);
			float* ptr = output->ptr<float>(i);

			for (size_t j = 0; j < image->cols; j++)
			{

				ptr[j] = a * log((b * row_ptr[j] / 256.0f) + 1.0f);
			}
		}
		auto func = [](uchar value, const int*) {
			//cout << value << " ";
		};
		image->forEach<uchar, decltype(func)>(func);
		return output;
	}


	void test_performance()
	{
		// Load Image
		const string colorImage = "./res/cat-1401564_1920.jpg";
		Mat I = imread(colorImage);
		//Mat f_Image = Mat(I.rows, I.cols, CV_32FC1);
		vector<Mat> rgb_channels;
		split(I, rgb_channels);
		Mat f_Image = rgb_channels[0];
		f_Image.convertTo(f_Image, CV_32FC1, 1.0f / 255.0f);

		cout << "________________________________" << endl;
		cout << "______TEST______________________" << endl;
		Mat_<Vec3b> _I = I.clone();
		double t = (double)getTickCount();
		for (int i = 0; i < _I.rows; i++)
		{
			for (int j = 0; j < _I.cols; j++)
			{
				Vec3b& _v3 = _I(i, j);
				_v3(0) = _v3(0) * 0.5 + 0.2;
				//_v3.all(0);
			}
		}
		cout << " Counter = " << (double)getTickCount() - t << endl;
		//namedWindow("calcHist Demo 1", CV_WINDOW_AUTOSIZE);
		//imshow("calcHist Demo 1", _I);
		//_I.release();

		_I = I.clone();

		t = (double)getTickCount();
		MatIterator_<Vec3b> it, end;
		for (it = _I.begin(), end = _I.end(); it != end; it++)
		{
			(*it)[0] = 0;
		}
		cout << " counter = " << (double)getTickCount() - t << endl;
		//namedWindow("calcHist Demo 2", CV_WINDOW_AUTOSIZE);
		//imshow("calcHist Demo 2", _I);
		_I.release();

		t = (double)getTickCount();
		auto func = [](Vec3b& value, const int*) {
			//value[0] = 1;
			value[1] = 0.0 * value[1] + 0.1;
			//value[2] = 0.0f;
		};
		I.forEach<Vec3b, decltype(func)>(func);
		//namedWindow("calcHist Demo 2", CV_WINDOW_AUTOSIZE);
		//imshow("calcHist Demo 2", I);

		cout << " Counter = " << ((double)getTickCount() - t) / getTickFrequency() << endl;
	}

	void video_capture()
	{
		VideoCapture cap(0); // open the default camera
		if (!cap.isOpened())  // check if we succeeded
			return;

		Mat edges;
		namedWindow("edges", 1);
		for (;;)
		{
			Mat frame;

			cap >> frame; // get a new frame from camera
			//cvtColor(frame, edges, COLOR_BGR2GRAY);
			//GaussianBlur(edges, edges, Size(7, 7), 1.5, 1.5);

			//Canny(edges, edges, 0, 30, 3);
			imshow("edges", frame);
			if (waitKey(30) >= 0) break;
		}
		// the camera will be deinitialized automatically in VideoCapture destructor
	}

	void Convolution(const Mat* H, const Mat* I, Mat** result)
	{
		int m = H->size().width;
		int n = H->size().height;
		int img_width = I->size().width;
		int img_height = I->size().height;
		// Check result is not null ptr
		assert(result != NULL);
		int _cols = (m - 1) + img_width; // Noted
		int _rows = (n - 1) + img_height; // Noted
		Mat _temp;
		_temp = Mat::zeros(2 * (m - 1) + img_width, 2 * (n - 1) + img_height, CV_32FC1);
		// Copy data
		_temp(Rect(m - 1, n - 1, m - 1 + img_width, n - 1 + img_height)) = *I;
		*result = new Mat(_rows, _cols, CV_32FC1);
		/*for (int i = 0; i < _cols; i++)
		{
			for (int i = 0; i < _cols; i++)
			{
				filter2D
			}
		}*/
	}

/*
int main(int agrc, char **argv)
{
	string filename = "./res/rice.png";
	Mat mImage = imread(filename, IMREAD_GRAYSCALE);
	//Mat dest = mImage.clone();
	//Mat* dest = LogTranformation(3.0f,  1.0, &mImage);

	//IplImage* pImage = cvLoadImage(filename.c_str());
	//// Create a Mat
	//Mat sData = Mat::zeros(mImage.size(), CV_8U);
	//sData(Rect(2, 2, 10, 10)) = 100;
	
	//cvRectangle(&mImage, cvPoint(30, 30), cvPoint(40, 40), cvScalarAll(255));

	// namedWindow("Display window", WINDOW_AUTOSIZE); // Create a window for display.
	// imshow("Display window", dest);                // Show our image inside it.
	//test_performance();

	video_capture();

	return 0;
	//CvMat* pMat = cvCreateMat(3, 3, CV_32FC1);
	//Mat cppMat = Mat::zeros(3, 3, CV_32FC1);
	//
	////float float_element_1_1 = CV_MAT_ELEM(*pMat, float, 1, 1);
	//cout << "Data sizeof : " << pMat->step * pMat->rows << endl;
	////memset(pMat->data.ptr, 0, pMat->step * pMat->rows);
	//
	//cvRectangle(pMat, cvPoint(0, 0), cvPoint(1, 0), cvScalar(1.0f), CV_FILLED);
	////cout << *pMat << endl;
	//cout << "Sum :" << endl;
	//cout << sum(pMat) << endl;
	//// Release memory
	//cvReleaseMat(&pMat);

	/// Display
	/// Display
	namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE);
	//imshow("calcHist Demo", *dest);
	//dest->release();
	//delete dest;
	waitKey(0);

	// Exit program
	return 0;

	//cout << mImage.channels() << endl;
	const string colorImage = "./res/jewel_hightlight.png";

	Mat src, dst;

	/// Load image
	src = imread(colorImage, 1);

	if (!src.data)
	{
		return -1;
	}

	/// Separate the image in 3 places ( B, G and R )
	vector<Mat> bgr_planes;
	split(src, bgr_planes);

	/// Establish the number of bins
	int histSize = 256;

	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 };
	const float* histRange = { range };

	bool uniform = true; bool accumulate = false;

	Mat b_hist, g_hist, r_hist;

	/// Compute the histograms:
	calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);

	// Draw the histograms for B, G and R
	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);

	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	/// Normalize the result to [ 0, histImage.rows ]
	normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

	/// Draw for each channel
	for (int i = 1; i < histSize; i++)
	{
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))),
			Scalar(255, 0, 0), 2, 8, 0);
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(g_hist.at<float>(i))),
			Scalar(0, 255, 0), 2, 8, 0);
		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(r_hist.at<float>(i))),
			Scalar(0, 0, 255), 2, 8, 0);
	}

	/// Display
	namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE);
	imshow("calcHist Demo", histImage);

	waitKey(0);

	return 0;
}
*/

