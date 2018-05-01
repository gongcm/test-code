#include <opencv2/opencv.hpp>

using namespace cv;
//int value = 10;
Mat I;
Mat I_src;
double g_alpha;
void on_Trackbar(int value,void * userdata)
{
	//printf("%d   \n",value);
	g_alpha = (double)value / 100;
	printf("g_alpha : %f \n",g_alpha);
		//_value = value / 10;
		for (int i = 0; i < I.rows; i++)
		{
			for (int j = 0; j < I.cols; j++)
			{
				for (int c = 0; c < 3; c++)
				{
					I.at<Vec3b>(i, j)[c] = (g_alpha* I_src.at<Vec3b>(i, j)[c] +(1 - g_alpha)* value);
				}
			}
		}

	imshow("test",I);
}
int main()
{
	// g(x) = alpha * f(x) + beat
	int g_Data;
	I_src = imread("D:\\Lena.jpg");
	I =Mat::zeros(I_src.size(), I_src.type());
	char info[256];
	int value = 50;
	sprintf_s(info,"¶Ô±È¶È %d",100);
	namedWindow("test");
	createTrackbar(info,"test",&value,100,on_Trackbar);

	waitKey(0);
	return 0;
}