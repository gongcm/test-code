#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include <iostream>
/*
	它是按图像的灰度特性,将图像分成背景和目标2部分。背景和目标之间的类间方差越大,
	说明构成图像的2部分的差别越大,当部分目标错分为背景或部分背景错分为目标都会导致2部分差别变小。
	因此,使类间方差最大的分割意味着错分概率最小。

	对于图像I(x,y),前景(即目标)和背景的分割阈值记作T,属于前景的像素点数占整幅图像的比
	例记为ω0,其平均灰度μ0;背景像素点数占整幅图像的比例为ω1,其平均灰度为μ1。图像的总平均
	灰度记为μ,类间方差记为g。
	假设图像的背景较暗,并且图像的大小为M×N,
	图像中像素的灰度值小于阈值T的像素个数记作N0,像素灰度大于阈值T的像素个数记作N1,则有:
	　　　　　　ω0=N0/ M×N (1)
		  　　　ω1=N1/ M×N (2)
				N0+N1=M×N (3)
				ω0+ω1=1 (4)
				μ=ω0*μ0+ω1*μ1 (5)
				g=ω0(μ0-μ)^2+ω1(μ1-μ)^2 (6)
				将式(5)代入式(6),得到等价公式: g=ω0ω1(μ0-μ1)^2 (7)
				采用遍历的方法得到使类间方差最大的阈值T,即为所求。
*/
double otus(IplImage * src)
{
	// src 为 灰度图片
	int width = src->width;
	int height = src->height;

	int hist[256];

	/*
		灰度图片的灰度值： 0—256
	*/
	for (int i = 0; i < height; i++)
	{
		unsigned char * p = (unsigned char *)src->imageData + src->widthStep * i;
		for (int j = 0; j <= 256; j++)
		{
			hist[*p++]++; // 统计每个灰度级别的个数。
		}
	}

	float average;// 整个图片的灰度平均值。
	float u = 0;
	for (int i = 0; i <= 256; i++)
	{
		u += hist[i]*i;
	}

	average = u / width * height;

	int size = width * height;
	float T;
	float w0 = 0;
	float w1;
	float average_f = 0;
	float average_b = 0;
	float maxvalu = 0, value = 0;
	for (int i = 0; i <= 256; i++)
	{
		w0 += hist[i];// i 为
		average_f += i*hist[i];
	
		float t = average_f / w0 - average;

		value = (w0/size )*(1 - w0/size)*t*t;

		if (value > maxvalu)
		{
			maxvalu = value;
			T = i;
		}
	}
	return T;
}
int main()
{
	
	cv::Mat mat,gray_img;
	mat = cv::imread("D:/Alistar.jpg",cv::IMREAD_COLOR);

	cv::imshow("image",mat);
	cv::cvtColor(mat,gray_img,cv::COLOR_BGR2GRAY);

	cv::imshow("gray",gray_img);
	IplImage  img(gray_img);
	int t = otus(&img);
	std::cout << "阈值 ： " << t << std::endl;
	cv::Mat img2;
	cv::Mat img1(gray_img);
	cv::threshold(img1,img2,t,0,cv::THRESH_BINARY| cv::THRESH_OTSU);

	cv::imshow("img2",img2);
	cv::waitKey(-1);
	
}