#include <opencv2/opencv.hpp>

using namespace cv;

int main()
{
	// 图像混合 ROI
	Mat A, B;
	double alpha = 0.7;
	double beta = 1 - alpha;
	A = imread("D:\\Lena.jpg");
	B = imread("D:\\Airplane.jpg");

	namedWindow("Lena");
	namedWindow("Airplane");

	imshow("Lena",A);
	imshow("Airplane", B);

	Mat A_im = A(Range(A.rows / 4, (A.rows - A.rows / 4)), Range(A.rows / 4, (A.rows - A.rows / 4)));
	Mat B_im = B(Range(A.rows / 4, (A.rows - A.rows / 4)), Range(A.rows / 4, (A.rows - A.rows / 4)));
	Mat img;
	//cvAddWeighted(&A_im,alpha,&B,beta,0,&img);
	// ROI  区域的A_IM B_IM 必须是同等大小的
	addWeighted(A_im, alpha, B_im, beta, 0, img);
	namedWindow("Blend");
	imshow("Blend", img);

	cvWaitKey(0);
}