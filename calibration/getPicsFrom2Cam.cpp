#include<opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{

    cout << 123 << endl;
	VideoCapture cap1;
	VideoCapture cap2;
	Mat p1;
	Mat p2;

	char f1[15];
    char f2[15];

    int i=1;
	while (1)
	{
        cap1.open(1);
        if (!cap1.isOpened())return 0;
		cap1 >> p1;
		imshow("p1", p1);
        cap1.release();

		cap2.open(2);
        if (!cap2.isOpened())return 0;
		cap2 >> p2;
		imshow("p2", p2);
        cap2.release();

        char c = waitKey(200);

        if (c == 27)
            break;
        else
            if(c == ' '){
                sprintf(f1, "%d_1_target.jpg",i);
                imwrite(f1, p1);
                sprintf(f2, "%d_2_target.jpg",i++);
                imwrite(f2, p2);
                cout << i-1 << "th frame" << endl;
            }

	}

	return 0;
}
