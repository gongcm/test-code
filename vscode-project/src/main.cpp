#include <iostream>
extern "C"{
    #include "opencv/cv.hpp"
}

#pragma once
#define WinMain
//using namespace cv;
int main()
{
    cv::Mat img;
    img = cv::imread("D:\Lena.jpg");
    cv::namedWindow("src");
    cv::imshow("src",img);
    cv::waitKey(0);
}
