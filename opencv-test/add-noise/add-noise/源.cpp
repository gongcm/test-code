#include <opencv2\opencv.hpp>
#include <random>

using namespace cv;
using namespace std;

// 添加椒盐噪声
void addSaltNoise(Mat &m, int num)
{
	// 随机数产生器
	std::random_device rd; //种子
	std::mt19937 gen(rd()); // 随机数引擎

	auto cols = m.cols * m.channels();

	for (int i = 0; i < num; i++)
	{
		auto row = static_cast<int>(gen() % m.rows);
		auto col = static_cast<int>(gen() % cols);

		auto p = m.ptr<uchar>(row);
		p[col++] = 255;
		p[col++] = 255;
		p[col] = 255;
	}
}

// 添加Gussia噪声
// 使用指针访问
void addGaussianNoise(Mat &m, int mu, int sigma)
{
	// 产生高斯分布随机数发生器
	std::random_device rd;
	std::mt19937 gen(rd());

	std::normal_distribution<> d(mu, sigma);

	auto rows = m.rows; // 行数
	auto cols = m.cols * m.channels(); // 列数

	for (int i = 0; i < rows; i++)
	{
		auto p = m.ptr<uchar>(i); // 取得行首指针
		for (int j = 0; j < cols; j++)
		{
			auto tmp = p[j] + d(gen);
			tmp = tmp > 255 ? 255 : tmp;
			tmp = tmp < 0 ? 0 : tmp;
			p[j] = tmp;
		}
	}
}

int main()
{
	Mat I = imread("D:\\Lena.jpg");

	if (I.empty())
	{
		cout << "error Load image \n";
		waitKeyEx(0);
	}
	
	namedWindow("src");

	imshow("src",I);

	Mat I_a = I.clone();

	addSaltNoise(I_a,I.rows);
	namedWindow("salt");
	imshow("salt",I_a);

	Mat I_b = I.clone();

	addGaussianNoise(I_b,0,1);
	namedWindow("GaussianNoise");
	imshow("GaussianNoise", I_b);

	waitKey(0);
}
