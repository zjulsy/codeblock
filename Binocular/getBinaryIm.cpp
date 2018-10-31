#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <map>
using namespace cv;
using namespace std;

int mainbin()
{
    string filename = "1_2_target.jpg";
    Mat p1 = imread(filename, IMREAD_GRAYSCALE );
    Mat p1_color = imread(filename, IMREAD_COLOR );
    imshow("original", p1);
    imshow("original_color", p1_color);
    Mat binary_p1;
    threshold(p1, binary_p1, 200, 255, THRESH_BINARY);
    imshow("binary", binary_p1);
    Mat open_p, close_p, element(3,3, CV_8U, Scalar(1));
    morphologyEx(binary_p1, open_p, MORPH_OPEN, element);
    morphologyEx(binary_p1, close_p, MORPH_CLOSE, element);
    imshow("open", open_p);
    imshow("close", close_p);

    //use close
    vector<vector<Point>> contours;
    findContours(close_p, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    Mat contours_M(close_p.size(), CV_8U, Scalar(255));
    drawContours(contours_M, contours, -1, 0, 1);
    imshow("contours", contours_M);
    cout << contours.size() << endl;


    //sort
    sort(contours.begin(), contours.end(), [](vector<Point> &a, vector<Point> &b) {
        return a.size() > b.size();
    });
    for(int i=0; i<contours.size(); ++i){
        cout << contours[i].size() << " ";
    }
    cout << endl;

    //select
    for(int i=contours.size()-1; i>3; --i){
        if(contours[i].size()<10)
            contours.pop_back();
        else
            break;
    }

    vector<RotatedRect> rrv;
    vector<vector<double>> v2;
    for(int i=0; i<contours.size(); ++i){
        cout << i << " : points " << contours[i].size() << ", ";
        double s1 = contourArea(contours[i]);
        RotatedRect rect = fitEllipse(Mat(contours[i]));
        rrv.push_back(rect);
        double s2 = 3.1415926*rect.size.area();
        cout << "contourArea " << s1 << ", " << "EllipseArea " << s2 << ", " << "diff " << abs(s1-s2)/s1 << endl;
        if(s1<=10 || s2 <=10)
            continue;
        v2.push_back({abs(s1-s2)/s1, i});
    }

    sort(v2.begin(), v2.end(), [](vector<double> &a, vector<double> &b) {
        return a[0] < b[0];
    });


    for(int i=0; i<v2.size(); ++i){
        cout << v2[i][1] << " ";
    }


    //draw circle
    //Mat qqq(close_p.size(), CV_8U, Scalar(255));
    for(int i=0; i<4; ++i){
        //ellipse(p1_color, rrv[(int)v2[i][1]], Scalar(255),2);
        cout << rrv[(int)v2[i][1]].center << " ";
    }
    ellipse(p1_color, rrv[(int)v2[0][1]], Scalar(255),2);
    ellipse(p1_color, rrv[(int)v2[1][1]], Scalar(0,255),2);
    ellipse(p1_color, rrv[(int)v2[2][1]], Scalar(0,0,255),2);
    ellipse(p1_color, rrv[(int)v2[3][1]], Scalar(255,0,255),2);
    cout << endl;
    imshow("getIts",p1_color);
    waitKey(0);
}

