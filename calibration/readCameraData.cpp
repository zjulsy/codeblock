#include <opencv2/core.hpp>
#include <iostream>
#include <string>
using namespace cv;
using namespace std;

int main3(int ac, char** av)
{

    string filename = "out_camera_data_l.xml";

    {//read
        cout << endl << "Reading: " << endl;
        FileStorage fs;
        fs.open(filename, FileStorage::READ);

        if (!fs.isOpened())
        {
            cerr << "Failed to open " << filename << endl;
            return 1;
        }

        Mat camera_matrix, distortion_coefficients;
        fs["camera_matrix"] >> camera_matrix;                                      // Read cv::Mat
        fs["distortion_coefficients"] >> distortion_coefficients;

        cout << endl
            << "camera_matrix = " << camera_matrix << endl;
        cout << "distortion_coefficients = " << distortion_coefficients << endl << endl;


    }

    return 0;
}
