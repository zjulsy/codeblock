#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
using namespace cv;
using namespace std;
bool calibrate(Mat& intrMat, Mat& distCoeffs, vector<vector<Point2f>>& imagePoints,
    vector<vector<Point3f>>& ObjectPoints, Size& imageSize,const int cameraId ,
    vector<string> imageList);
static void calcChessboardCorners(Size boardSize, float squareSize, vector<Point3f>& corners);

int mainstereo()
{
    //initialize some parameters
    bool okcalib = false;
    Mat intrMatFirst, intrMatSec, distCoeffsFirst, distCoffesSec;
    Mat R, T, E, F, RFirst, RSec, PFirst, PSec, Q;
    vector<vector<Point2f>> imagePointsFirst, imagePointsSec;
    vector<vector<Point3f>> ObjectPoints(1);
    Rect validRoi[2];
    Size imageSize;
    int cameraIdFirst = 0, cameraIdSec = 1;
    double rms = 0;

    //get pictures and calibrate
    vector<string> imageList;
    FileStorage fs;
    fs.open("pics.xml", FileStorage::READ);
    FileNode n = fs["images"];
    FileNodeIterator it = n.begin(), it_end = n.end(); // Go through the node
        for (; it != it_end; ++it)
            imageList.push_back((string)(*it));

    fs.open("stereoCalib.xml", FileStorage::WRITE);
    //calibrate
    cout << "calibrate left camera..." << endl;
    okcalib = calibrate(intrMatFirst, distCoeffsFirst, imagePointsFirst, ObjectPoints,
        imageSize, cameraIdFirst, imageList);

    if (!okcalib)
    {
        cout << "fail to calibrate left camera" << endl;
        return -1;
    }
    else
    {
        fs << "K1" << intrMatFirst;
        fs << "D1" << distCoeffsFirst;
        cout << "calibrate the right camera..." << endl;
    }

    okcalib = calibrate(intrMatSec, distCoffesSec, imagePointsSec, ObjectPoints,
        imageSize, cameraIdSec, imageList);

    if (!okcalib)
    {
        cout << "fail to calibrate the right camera" << endl;
        return -1;
    }
    fs << "K2" << intrMatSec;
    fs << "D2" << distCoffesSec;

    destroyAllWindows();

    //estimate position and orientation
    cout << "estimate position and orientation of the second camera" << endl
        << "relative to the first camera..." << endl;
    rms = stereoCalibrate(ObjectPoints, imagePointsFirst, imagePointsSec,
        intrMatFirst, distCoeffsFirst, intrMatSec, distCoffesSec,
        imageSize, R, T, E, F, CV_CALIB_FIX_INTRINSIC,
        TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 30, 1e-6));

    fs << "R" << R;
    fs << "T" << T;
    fs.release();
    cout << "done with RMS error=" << rms << endl;

}

bool calibrate(Mat& intrMat, Mat& distCoeffs, vector<vector<Point2f>>& imagePoints,
    vector<vector<Point3f>>& ObjectPoints, Size& imageSize,const int cameraId ,
    vector<string> imageList)
{
    int w = 6;
    int h = 9;
    double rms = 0;

    Size boardSize;
    boardSize.width = w;
    boardSize.height = h;
    vector<Point2f> pointBuf;
    float squareSize = 28.8f;
    vector<Mat> rvecs, tvecs;
    bool ok = false;

    int nImages = (int)imageList.size() / 2;
    namedWindow("View", 1);
    for (int i = 0; i<nImages ; i++)
    {
        Mat view, viewGray;
        view = imread(imageList[i*2+cameraId], 1);
        imageSize = view.size();
        cvtColor(view, viewGray, COLOR_BGR2GRAY);

        bool found = findChessboardCorners(view, boardSize, pointBuf,
            CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);

        if (found)
        {
            cornerSubPix(viewGray, pointBuf, Size(11, 11),
                Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
            drawChessboardCorners(view, boardSize, Mat(pointBuf), found);
            bitwise_not(view, view);
            imagePoints.push_back(pointBuf);
            cout << '.';
        }
        imshow("View", view);
        waitKey(50);
    }
    //calculate chessboardCorners
    calcChessboardCorners(boardSize, squareSize, ObjectPoints[0]);
    ObjectPoints.resize(imagePoints.size(), ObjectPoints[0]);

    rms = calibrateCamera(ObjectPoints, imagePoints, imageSize, intrMat, distCoeffs,
        rvecs, tvecs);
    ok = checkRange(intrMat) && checkRange(distCoeffs);

    if (ok)
    {
        cout << "done with RMS error=" << rms << endl;
        return true;
    }
    else
        return false;
}

static void calcChessboardCorners(Size boardSize, float squareSize, vector<Point3f>& corners)
{
    corners.resize(0);
    for (int i = 0; i < boardSize.height; i++)        //height和width位置不能颠倒
    for (int j = 0; j < boardSize.width; j++)
    {
        corners.push_back(Point3f(j*squareSize, i*squareSize, 0));
    }
}
