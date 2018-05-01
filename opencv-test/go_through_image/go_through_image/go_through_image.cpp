#include <opencv2\opencv.hpp>


using namespace cv;
using namespace std;

void test(Mat & I)
{
	int i, j;
	for (i = 0; i < I.rows; i++)
	{
		for (j = 0; j < I.cols; j++)
		{
			Vec3b pixel = I.at<Vec3b>(i, j);
			I.at<Vec3b>(i,j)[0] = pixel[0] / 4 * 4;
			I.at<Vec3b>(i, j)[1] = pixel[1] / 4 * 4;
			I.at<Vec3b>(i, j)[2] = pixel[2] / 4 * 4;
		}
	}
	
}

Mat& ScanImageAndReduceC(Mat& I,uchar * table)
{
	//Mat img = I.clone();
	int nRows = I.rows;
	int nCols = I.cols * I.channels();

	if (I.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}

	int i, j;
	uchar *p = NULL;
	for (i = 0; i < nRows; i++)
	{
		p = I.ptr<uchar>(i);
		for (j = 0; j < nCols; j++) 
		{
			p[j] = table[p[j]];
		}
	}

	return I;
}

Mat & ScanImageAndReduceIterator(Mat&I,uchar * table)
{
	int i = I.channels();
	switch (i)
	{
		case 1:
		{
			MatIterator_<uchar> it, end;
			for (it = I.begin<uchar>(), end = I.end<uchar>(); it != end; ++it)
			{
				*it = table[*it];
			}

			break;
		}
		case 3:
		{
			MatIterator_<Vec3b> it, end;
			for (it = I.begin<Vec3b>(), end = I.end<Vec3b>(); it != end; ++it)
			{
				(*it)[0] = table[(*it)[0]];
				(*it)[1] = table[(*it)[1]];
				(*it)[2] = table[(*it)[2]];
			}
		
		}
	}
	return I;
}

Mat& ScanImageAndReduceRandomAccess(Mat& I, const uchar* const table)
{
	// accept only char type matrices
	CV_Assert(I.depth() == CV_8U);
	const int channels = I.channels();
	switch (channels)
	{
	case 1:
	{
		for (int i = 0; i < I.rows; ++i)
			for (int j = 0; j < I.cols; ++j)
				I.at<uchar>(i, j) = table[I.at<uchar>(i, j)];
		break;
	}
	case 3:
	{
		Mat_<Vec3b> _I = I;
		for (int i = 0; i < I.rows; ++i)
			for (int j = 0; j < I.cols; ++j)
			{
				_I(i, j)[0] = table[_I(i, j)[0]];
				_I(i, j)[1] = table[_I(i, j)[1]];
				_I(i, j)[2] = table[_I(i, j)[2]];
			}
		I = _I;
		break;
	}
	}
	return I;
}

void Sharpen(const Mat& myImage, Mat& Result)
{
	CV_Assert(myImage.depth() == CV_8U);  // accept only uchar images
	const int nChannels = myImage.channels();
	Result.create(myImage.size(), myImage.type());
	for (int j = 1; j < myImage.rows - 1; ++j)
	{
		const uchar* previous = myImage.ptr<uchar>(j - 1);
		const uchar* current = myImage.ptr<uchar>(j);
		const uchar* next = myImage.ptr<uchar>(j + 1);
		uchar* output = Result.ptr<uchar>(j);
		for (int i = nChannels; i < nChannels*(myImage.cols - 1); ++i)
		{
			*output++ = saturate_cast<uchar>(5 * current[i]
				- current[i - nChannels] - current[i + nChannels] - previous[i] - next[i]);
		}
	}
	Result.row(0).setTo(Scalar(0));
	Result.row(Result.rows - 1).setTo(Scalar(0));
	Result.col(0).setTo(Scalar(0));
	Result.col(Result.cols - 1).setTo(Scalar(0));
}

int main()
{
	uchar table[256];
	int diviedWidth = 25;
	Mat I = imread("D:\\Lena.jpg");

	cvNamedWindow("Lena");
	imshow("Lena",I);

	for (int i = 0; i < 256; ++i)
	{
		table[i] = (uchar)(diviedWidth *(i / diviedWidth));
	//	printf("\ntable : %d\b\b",table[i]);
	}

	//cout << "table" << table << endl;
	double t = (double)getTickCount();
	Mat A = ScanImageAndReduceC(I,table);
	double time = (((double)getTickCount() - t) / getTickFrequency());
	printf("ScanImageAndReduceC time : %f \n",time);
	namedWindow("A");
	imshow("A",A);
	t = (double)getTickCount();
	Mat B = ScanImageAndReduceIterator(I, table);
	 time = (((double)getTickCount() - t) / getTickFrequency());
	 printf("ScanImageAndReduceIterator time : %f \n", time);
	namedWindow("B");
	imshow("B", B);

	t = (double)getTickCount();
	Mat C = ScanImageAndReduceRandomAccess(I, table);
	time = (((double)getTickCount() - t) / getTickFrequency());
	printf("ScanImageAndReduceRandomAccess time : %f \n", time);
	namedWindow("C");
	imshow("C", C);
	Mat result;
	Sharpen(I.clone(),result);
	namedWindow("Sharpen");
	imshow("Sharpen", result);

	Mat kernel = (Mat_<char>(3, 3) << 0, -1, 0,
		-1, 5, -1,
		0, -1, 0);
	Mat out_img;
	filter2D(I.clone(),out_img,I.depth(),kernel);

	namedWindow("filter2D");
	imshow("filter2D", out_img);

	test(I);
	namedWindow("Test");
	imshow("Test",I);

	
	cvWaitKey(0);
}