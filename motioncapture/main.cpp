#include "functions.h"

#include <cstdlib>
#include <iostream>
#include <QtGlobal>
#include <cmath>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;

int main( ) {
//    VideoCapture cap(0);
    string path = "../outA.mkv";
    VideoCapture cap( path );
    if(!cap.isOpened())
        return -1;

//    int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
//    int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
//    VideoWriter video("../outA.avi",
//                      CV_FOURCC('M','J','P','G'),
//                      10,
//                      Size( frame_width,frame_height ),
//                      true);

    namedWindow("main",1);
    Mat frame;
    ECircle type;

    PathType _pathBL, _pathBR, _pathUL, _pathUR; // bottom left, bottom right, upper left, upper right
    PointsType circles;
    circles.reserve( NUMBER_OF_CIRCLES );
    int xs[ NUMBER_OF_CIRCLES ];
    int ys[ NUMBER_OF_CIRCLES ];
    bool F_FIRST;
    while( cap.read(frame) ) {
        cvtColor(frame, frame, CV_BGR2GRAY); // convert to grayscale
        GaussianBlur( frame, frame, Size(3, 3), 2, 2 ); // blur
        threshold(frame, frame, 163, 255, THRESH_BINARY); // binarisation

        erode(frame, frame, Mat(), Point(-1, -1), 3); // erosion
        dilate(frame, frame, Mat(), Point(-1, -1), 3); // dilation

        vector<PathType> contours;
        vector<Vec4i> hierarchy;
        findContours( frame, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) ); // find contours of things

        cvtColor(frame, frame, CV_GRAY2BGR);
        Point2f* centers = new Point2f[ contours.size() ];
        float* radiuses = new float[ contours.size() ];
        circles.clear();
        for (u_int i = 0; i < contours.size(); i++) {
            minEnclosingCircle(contours[i], centers[i], radiuses[i]);
            if ( validateCircle( radiuses[i], contours[i] ) ) {
                circle( frame, centers[i], radiuses[i], Scalar(255, 0, 0), 2, 8, 0 );
                circles.push_back( centers[i] );
            }
        }

        if( F_FIRST == true && circles.size() == NUMBER_OF_CIRCLES ) {
            for (u_int i = 0; i < circles.size(); i++) {
                xs[i] = circles[i].x;
                ys[i] = circles[i].y;
            }
            qsort(xs, NUMBER_OF_CIRCLES, sizeof(int), compare);
            qsort(ys, NUMBER_OF_CIRCLES, sizeof(int), compare);
            // add initial points to proper vectors
            for (u_int i = 0; i < circles.size(); i++) {
                if ( (circles[i].x == xs[0] || circles[i].x == xs[1]) &&
                     (circles[i].y == ys[0] || circles[i].y == ys[1]) ) {
                    _pathBL.push_back(circles[i]);
                }
                else if ( (circles[i].x == xs[0] || circles[i].x == xs[1]) &&
                          (circles[i].y == ys[2] || circles[i].y == ys[3]) ) {
                    _pathBR.push_back(circles[i]);
                }
                else if ( (circles[i].x == xs[2] || circles[i].x == xs[3]) &&
                          (circles[i].y == ys[0] || circles[i].y == ys[1]) ) {
                    _pathUL.push_back(circles[i]);
                }
                else if ( (circles[i].x == xs[2] || circles[i].x == xs[3]) &&
                          (circles[i].y == ys[2] || circles[i].y == ys[3]) ) {
                    _pathUR.push_back(circles[i]);
                }
            }
            F_FIRST = false;
        }
        else {
            // add positions to the corresponding vectors
            for (u_int i = 0; i < circles.size(); i++) {
                type = findCircle(circles[i], _pathBL, _pathBR, _pathUL, _pathUR);
                if( type == UR )
                    _pathUR.push_back( circles[i] );
                else if( type == UL )
                    _pathUL.push_back( circles[i] );
                else if( type == BR )
                    _pathBR.push_back( circles[i] );
                else if( type == BL )
                    _pathBL.push_back( circles[i] );
            }
        }

        drawPath(_pathUR, Scalar(0, 255, 255), frame); // orange
        drawPath(_pathUL, Scalar(255, 0, 0), frame); // blue
        drawPath(_pathBL, Scalar(0, 0, 255), frame); // red
        drawPath(_pathBR, Scalar(0, 255, 0), frame); // green

//        HoughCircles(frame, circles, CV_HOUGH_GRADIENT, 1, frame.rows/8, 200, 100, 0, 0 );
//        for( size_t i = 0; i < circles.size(); i++) {
//            Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
//                  int radius = cvRound(circles[i][2]);
//                  circle( frame, center, 3, Scalar(0,255,0), -1, 8, 0 );
//                  circle( frame, center, radius, Scalar(0,0,255), 3, 8, 0 );
//        }
//        if( circles.size() ) cout << "znalezionych okregow: " << circles.size();
        //video.write(frame);
        imshow("main", frame);
        if(waitKey(30) >= 0) break;
    }
    return 0;
}
