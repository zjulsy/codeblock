//[306.163, 145.137] [277.414, 251.551] [241.031, 180.446] [343.389, 90.5142]
// [241.878, 105.263] [145.993, 196.257] [206.5, 160.5] [182.248, 265.55]
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <opencv2/calib3d.hpp>
using namespace cv;
using namespace std;
// triangulate using Linear LS-Method
cv::Vec3d triangulate(const cv::Mat &p1, const cv::Mat &p2, const cv::Vec2d &u1, const cv::Vec2d &u2) {

	// system of equations assuming image=[u,v] and X=[x,y,z,1]
	// from u(p3.X)= p1.X and v(p3.X)=p2.X
	cv::Matx43d A(u1(0)*p1.at<double>(2, 0) - p1.at<double>(0, 0), u1(0)*p1.at<double>(2, 1) - p1.at<double>(0, 1), u1(0)*p1.at<double>(2, 2) - p1.at<double>(0, 2),
		u1(1)*p1.at<double>(2, 0) - p1.at<double>(1, 0), u1(1)*p1.at<double>(2, 1) - p1.at<double>(1, 1), u1(1)*p1.at<double>(2, 2) - p1.at<double>(1, 2),
		u2(0)*p2.at<double>(2, 0) - p2.at<double>(0, 0), u2(0)*p2.at<double>(2, 1) - p2.at<double>(0, 1), u2(0)*p2.at<double>(2, 2) - p2.at<double>(0, 2),
		u2(1)*p2.at<double>(2, 0) - p2.at<double>(1, 0), u2(1)*p2.at<double>(2, 1) - p2.at<double>(1, 1), u2(1)*p2.at<double>(2, 2) - p2.at<double>(1, 2));

	cv::Matx41d B(p1.at<double>(0, 3) - u1(0)*p1.at<double>(2, 3),
		          p1.at<double>(1, 3) - u1(1)*p1.at<double>(2, 3),
		          p2.at<double>(0, 3) - u2(0)*p2.at<double>(2, 3),
		          p2.at<double>(1, 3) - u2(1)*p2.at<double>(2, 3));

	// X contains the 3D coordinate of the reconstructed point
	cv::Vec3d X;

	// solve AX=B
	cv::solve(A, B, X, cv::DECOMP_SVD);

	return X;
}

// triangulate a vector of image points
void triangulate(const cv::Mat &p1, const cv::Mat &p2, const std::vector<cv::Vec2d> &pts1, const std::vector<cv::Vec2d> &pts2, std::vector<cv::Vec3d> &pts3D) {

	for (int i = 0; i < pts1.size(); i++) {

		pts3D.push_back(triangulate(p1, p2, pts1[i], pts2[i]));
	}
}

double getDist(Vec3d a, Vec3d b){
    return norm(a,b);
}

int maintri()
{
string filename = "stereoCalib.xml";

    {//read
        cout << endl << "Reading: " << endl;
        FileStorage fs;
        fs.open(filename, FileStorage::READ);

        if (!fs.isOpened())
        {
            cerr << "Failed to open " << filename << endl;
            return 1;
        }

        Mat K1, D1, K2, D2, R, T;
        fs["K1"] >> K1;                                      // Read cv::Mat
        fs["D1"] >> D1;
        fs["K2"] >> K2;
        fs["D2"] >> D2;
        fs["R"] >> R;
        fs["T"] >> T;

        Mat P1(3,4,CV_64F), P2(3,4,CV_64F);
        Mat diag(Mat::eye(3,3,CV_64F));
        diag.copyTo(P1(Rect(0,0,3,3)));
        R.copyTo(P2(Rect(0,0,3,3)));
        T.copyTo(P2.colRange(3,4));

        vector<Vec2d> ups1, ups2;
        vector<Vec2d> ps1{{306.163, 145.137}, {277.414, 251.551}, {241.031, 180.446}, {343.389, 90.5142} };
        //vector<Vec2d> ps2{{241.878, 105.263}, {145.993, 196.257}, {206.5, 160.5}, {182.248, 265.55} };
        vector<Vec2d> ps2{ {206.5, 160.5}, {182.248, 265.55}, {145.993, 196.257}, {241.878, 105.263}};
;        //cout << ps1[1].val[1];
        //cout << T.at<float>
        //cout << format(T, Formatter::FMT_NUMPY ) ;
        //cout << ps1.size() << endl;
        //cout << T.at<double>(1);
        //Mat_<double> TT = T;
        double a1=T.at<double>(0), a2=T.at<double>(1), a3=T.at<double>(2);
        Mat F(3,3,CV_64F), K1_inv, K2_inv, Tx=(Mat_<double>(3,3)<< 0, -a3, a2, a3, 0, -a1, -a2, a1, 0);
        //cout << Tx;
        K1_inv = K1.inv();
        K2_inv = K2.inv();
        F = K2_inv.t()*Tx*R*K1_inv;
        //cout << F;
        /*
        Mat ps1_1=(Mat_<double>(3,1)<<343.389, 90.5142,1);
        Mat ps2_1=(Mat_<double>(3,1)<<241.878, 105.263,1), ps2_2=(Mat_<double>(3,1)<<145.993, 196.257,1);
        Mat ps2_3=(Mat_<double>(3,1)<<206.5, 160.5,1), ps2_4=(Mat_<double>(3,1)<<182.248, 265.55,1);
        Mat r11 = ps1_1.t()*F* ps2_1;
        cout << r11 << endl;
        r11 = ps1_1.t()*F* ps2_2;
        cout << r11 << endl;
        r11 = ps1_1.t()*F* ps2_3;
        cout << r11 << endl;
        r11 = ps1_1.t()*F* ps2_4;
        cout << r11 << endl;
        */

        undistortPoints(ps1, ups1, K1, D1);
        undistortPoints(ps2, ups2, K2, D2);
        vector<Vec3d> ps3d;
        //Mat temp1 = (Mat_<double>(2,4)<< 306.163, 145.137, 277.414, 251.551, 241.031, 180.446, 343.389, 90.5142);
        //Mat temp2 = (Mat_<double>(2,4)<< 206.5, 160.5, 182.248, 265.55, 145.993, 196.257, 241.878, 105.263);
        triangulate(P1, P2, ups1, ups2,ps3d);
        //cout << Mat(ps3d) ;
        Mat dist(4,4,CV_64F);
        for(int i=0; i<4; ++i)
            for(int j=i+1; j<4; ++j){
                dist.at<double>(i,j) = getDist(ps3d[i], ps3d[j]);
            }
        cout << format(dist, Formatter::FMT_NUMPY );
    }
}
