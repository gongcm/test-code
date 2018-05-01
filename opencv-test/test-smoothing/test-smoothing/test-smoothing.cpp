#include <opencv2\opencv.hpp>
#include <string>

using namespace std;
using namespace cv;

int main()
{
	//string filename = "";
	//IplImage * img;
	//C:\Users\Administrator\Pictures
	//img = cvLoadImage("C:\\Users\\Administrator\\Pictures\\Lena.jpg",CV_LOAD_IMAGE_UNCHANGED);
	//if (img)
	//{
	//	cout << "cvLoadimage Failed" << endl;
	//	return 0;
	//}
	//namedWindow("src");

	
	Mat img = imread("D:\\Lena.jpg");

	Mat D(img, Rect(10, 10, 100, 100));
	namedWindow("D");
	imshow("D",D);

	Mat B =img(Range::all(),Range::all());
	namedWindow("B");
	imshow("B", B);
	//IplImage * test = cvLoadImage("D:\\Lena.jpg",CV_LOAD_IMAGE_ANYCOLOR);
	namedWindow("src");
	
	cout << "width : " << img.rows << "\nheight :" << img.cols << endl;
	//cvWaitKey(0);
	imshow("src",img);
	
	Mat grey;
	cvtColor(img, grey, COLOR_BGR2GRAY);
	Mat sobelx;
	Sobel(grey, sobelx, CV_32F, 1, 0);
	double minVal, maxVal;
	minMaxLoc(sobelx, &minVal, &maxVal); //find minimum and maximum intensities
	Mat draw;
	sobelx.convertTo(draw, CV_8U, 255.0 / (maxVal - minVal), -minVal * 255.0 / (maxVal - minVal));
	namedWindow("image", WINDOW_AUTOSIZE);
	imshow("image", draw);
	waitKey();

	cout << "type : " << img.type() << endl;

//	Mat s = cvarrToMat(test); // 旧的数据结构和新的数据结构 
	
	//cvShowImage("Test", &s);
	Mat blurmat = img.clone();
	//Mat src(img);
	char buf[256];
	for (int i = 1; i < img.rows; i = i+5)
	{
		blur(img, blurmat, Size(i, i), Point(-1, -1), 4);
	//	cvWaitKey(0);
		namedWindow("blur");
		imshow("blur", blurmat);
		sprintf_s(buf,"D:\\backup\\%d-im.jpg",i);
		cout << "saved" << buf << endl;
		imwrite(buf,blurmat);
	}

	

	cvWaitKey(0);
}