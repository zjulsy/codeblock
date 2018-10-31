#include<opencv2/opencv.hpp>
#include<thread>
using namespace cv;

int maintest()
{

	VideoCapture cap(1);
	VideoCapture capcap(2);
	Mat frame;
	Mat frame2;
	if (!capcap.isOpened())return 0;
	if (!cap.isOpened())return 0;
	while (waitKey(30) != 27)
	{
		capcap >> frame2;
		imshow("摄像头2", frame2);
		cap >> frame;
		imshow("摄像头1", frame);
	}
	return 0;
}
