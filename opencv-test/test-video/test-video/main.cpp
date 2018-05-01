#include<iostream>  
#include<opencv2/opencv.hpp>  
#include <stdlib.h>
#include "cloniing_demo.h"
using namespace std;
using namespace cv;

/*
	遍历图像中像素点 RGB 三通道
*/
void colorReduce(Mat &image,int div)
{
	for(int i = 0;i < image.rows;i++)
	{
		for (int j = 0; j <= image.cols; j++)
		{
			image.at<Vec3b>(i,j)[0] = image.at<Vec3b>(i, j)[0] / div*div + div / 2;
			image.at<Vec3b>(i, j)[1] = image.at<Vec3b>(i, j)[1] / div*div + div / 2;
			image.at<Vec3b>(i, j)[2] = image.at<Vec3b>(i, j)[2] / div*div + div / 2;
		}
	}
}


void toHist(Mat &image)
{
	MatND hist;
	int channels[1] = {0};
	int histsize[1] = {256};
	float hranges[2] = {0,255};
	float * ranges = { hranges };
	Mat result = image.clone();

	cvtColor(result,result,CV_RGB2GRAY);

	//imshow("result", result);
	//calcHist(&Image, 1, channels, Mat(), hist, 1, histSize, ranges);
//	calcHist(&result,1,channels,Mat(),hist,1,histsize,ranges,true,false);

}
#if 0
int main()
{
	VideoCapture capture("test.avi");

	
	if (!capture.isOpened())
	{
		cout << "something is error !!" << endl;
		//cvvWaitKeyEx(0);
		cvWaitKey(0);
		return -1;
	}

	long frameNum = capture.get(CV_CAP_PROP_FRAME_COUNT);
	double rate = capture.get(CV_CAP_PROP_FPS);

	cout << "frame numbers :" << frameNum << endl;
	cout << "frame rate:" << rate << endl;

	cvWaitKey(0);
	namedWindow("Test");
	Mat frame;
	Mat oImage;
	long n = 0;
	int delay = 1000 / rate;
	while (n <= frameNum)
	{
		capture >> frame;
		oImage.create(frame.rows,frame.cols,CV_8U);
		//cvtColor(frame,oImage,);
		//colorReduce(frame,64);
		imshow("Test",frame);
	//	cvWaitKey(0);
		
	

		n++;
	}
	//while (1);
	return 0;
}
#else 
int  main()
{
	cloning_demo();
}

#endif
