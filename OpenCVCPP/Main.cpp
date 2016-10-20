#include "FourierTransform.h"
#include "HoughLineTransform.h"
#include "HistogramMatch.h"
#include <opencv2\opencv.hpp>
using namespace cv;
Mat g_Img;
Mat dest;
int low_threshold = 1;
int radio = 3;
void CannyThreshold(int, void*);
// OpenCV function
void test_fourier_transform(const char *filename) {
	cv::Mat I = cv::imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
	Mat padded;
	std::cout << "Image ogrin size = ( " << I.rows << " , " << I.cols << std::endl;
	
	int m = getOptimalDFTSize(I.rows);
	int n = getOptimalDFTSize(I.cols);
	std::cout << "Padded image size = ( " << m << " ,  " << n << " )." << std::endl;
	copyMakeBorder(I, padded, 0, m - I.rows, 0, n - I.cols, BORDER_CONSTANT, Scalar::all(0.0));
	
	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat ComplexI;
	merge(planes, 2, ComplexI);

	dft(ComplexI, ComplexI);
	split(ComplexI, planes);
	magnitude(planes[0], planes[1], planes[0]);
	Mat magI = planes[0];
	// Logarithmic scale
	magI += Scalar::all(1);
	log(magI, magI);

	// Crop and rearrage
	magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));
	int cx = magI.cols / 2;
	int cy = magI.rows / 2;

	Mat q0(magI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
	Mat q1(magI, Rect(cx, 0, cx, cy));  // Top-Right
	Mat q2(magI, Rect(0, cy, cx, cy));  // Bottom-Left
	Mat q3(magI, Rect(cx, cy, cx, cy)); // Bottom-Right

	Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
	q2.copyTo(q1);
	tmp.copyTo(q2);

	normalize(magI, magI, 0, 1, CV_MINMAX); // Transform the matrix with float values into a
											// viewable image form (float between values 0 and 1).
	g_Img = I;
	dest = Mat(g_Img.size(), g_Img.type());
	imshow("Input Image", dest);    // Show the result
	int val;
	
	createTrackbar("Threshold trackbar", "Input Image", &low_threshold, 100, CannyThreshold);
	imshow("spectrum magnitude", magI);
	waitKey();

}

void CannyThreshold(int, void*) {
	Mat detected_edges;
	
	blur(g_Img, detected_edges, Size(3, 3)); // Use boxfiltering
	Canny(detected_edges, detected_edges, low_threshold, low_threshold * radio);
	dest = Scalar::all(0);
	detected_edges.copyTo(dest);
	imshow("Input Image", dest);
}

// My function

int main(int argc, char *argv[])
{
	//const char filename[] = "./res/cursive-number-7.jpg";
	//test_fourier_transform(filename);

	//const CComplex test[] = { 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0 };
	//CArray data(test, 8);

	//// forward fft
	//fft(data);

	//std::cout << "fft" << std::endl;
	//for (int i = 0; i < 8; ++i)
	//{
	//	std::cout << data[i] << std::endl;
	//}

	//// inverse fft
	//ifft(data);

	//std::cout << std::endl << "ifft" << std::endl;
	//for (int i = 0; i < 8; ++i)
	//{
	//	std::cout << data[i] << std::endl;
	//}
	//test_blur();
	//demo_erosion();
	DemoLineTransform();
	return 0;
}