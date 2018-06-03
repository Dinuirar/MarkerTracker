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

bool modeWrite = false; // if this flag is set, the program generates video
bool modeLive = false; // if this flag is set, the program captures the video from default video device
bool modeValidate = false; // if this flag is set, the program prints the position of selected marker to standard output

int main( ) {
    VideoCapture cap;
    string path;
    if (modeLive)
        cap = VideoCapture(0);
    else {
        path = "../outA.mkv";
        cap = VideoCapture( path );
        if( !cap.isOpened() )
            return -1;
    }

    int frame_width, frame_height;
    VideoWriter video;
    if (modeWrite == true) {
        frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
        frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
        video = VideoWriter("../outB.avi",
                          CV_FOURCC('M','J','P','G'),
                          10,
                          Size( frame_width,frame_height ),
                          true);
    }

    u_int marker = 4;
    if( modeValidate ) {
        cout << "Choose marker ("
                "1 - bottom left, "
                "2 - bottom right, "
                "3 - upper left, "
                "4 - upper right):";
        cin >> marker;
    }

    namedWindow("main",1);
    Mat frame, outframe;
    ECircle type;
    PathType _pathUL_, _pathBL_, _pathUR_, _pathBR_; // bottom left, bottom right, upper left, upper right
    PointsType circles;
    circles.reserve( NUMBER_OF_CIRCLES );
    int xs[ NUMBER_OF_CIRCLES ];
    int ys[ NUMBER_OF_CIRCLES ];
    bool F_FIRST = true;
    path_el path_tmp;
    path_tmp.frame = 0;
    while( cap.read(frame) ) {
        outframe = frame.clone();
        path_tmp.frame++;
        cvtColor(frame, frame, CV_BGR2GRAY); // convert to grayscale
        GaussianBlur( frame, frame, Size(3, 3), 2, 2 ); // blur
        threshold(frame, frame, 163, 255, THRESH_BINARY); // binarisation

        erode(frame, frame, Mat(), Point(-1, -1), 3); // erosion
        dilate(frame, frame, Mat(), Point(-1, -1), 3); // dilation

        vector<PointsType> contours;
        vector<Vec4i> hierarchy;
        findContours( frame, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) ); // find contours of things

        cvtColor(frame, frame, CV_GRAY2BGR);
        Point2f* centers = new Point2f[ contours.size() ];
        float* radiuses = new float[ contours.size() ];
        circles.clear();
        for (u_int i = 0; i < contours.size(); i++) {
            minEnclosingCircle(contours[i], centers[i], radiuses[i]);
            if ( validateCircle( radiuses[i], contours[i] ) ) {
                circle( outframe, centers[i], radiuses[i], Scalar(255, 0, 0), 2, 8, 0 );
                circles.push_back( centers[i] );
            }
        }

        if( F_FIRST == true && circles.size() == NUMBER_OF_CIRCLES ) { // todo: add circles position validation for initialization
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
                    path_tmp.point = circles[i];
                    _pathUL_.push_back(path_tmp);
                }
                else if ( (circles[i].x == xs[0] || circles[i].x == xs[1]) &&
                          (circles[i].y == ys[2] || circles[i].y == ys[3]) ) {
                    path_tmp.point = circles[i];
                    _pathBL_.push_back(path_tmp);
                }
                else if ( (circles[i].x == xs[2] || circles[i].x == xs[3]) &&
                          (circles[i].y == ys[0] || circles[i].y == ys[1]) ) {
                    path_tmp.point = circles[i];
                    _pathUR_.push_back(path_tmp);
                }
                else if ( (circles[i].x == xs[2] || circles[i].x == xs[3]) &&
                          (circles[i].y == ys[2] || circles[i].y == ys[3]) ) {
                    path_tmp.point = circles[i];
                    _pathBR_.push_back(path_tmp);
                }
            }
            F_FIRST = false;
        }
        else {
            // add positions to the corresponding vectors
            for (u_int i = 0; i < circles.size(); i++) {
                type = findCircle(circles[i], _pathUL_, _pathBL_, _pathUR_, _pathBR_);
                if( type == UR ) {
                    path_tmp.point = circles[i];
                    _pathBR_.push_back( path_tmp );
                }
                else if( type == UL ) {
                    path_tmp.point = circles[i];
                    _pathUR_.push_back( path_tmp );
                }
                else if( type == BR ) {
                    path_tmp.point = circles[i];
                    _pathBL_.push_back( path_tmp );
                }
                else if( type == BL ) {
                    path_tmp.point = circles[i];
                    _pathUL_.push_back( path_tmp );
                }
            }
        }

        filterPath(_pathUR_);
        filterPath(_pathUL_);
        filterPath(_pathBR_);
        filterPath(_pathBL_);

        drawPath(_pathBR_, Scalar(0, 255, 255), outframe); // orange
        drawPath(_pathUR_, Scalar(255, 0, 0), outframe); // blue
        drawPath(_pathUL_, Scalar(0, 0, 255), outframe); // red
        drawPath(_pathBL_, Scalar(0, 255, 0), outframe); // green

        // pattern recognition based on points in paths
        if (marker == 1) { // bottom left
            printPosition( recognizePosition(_pathBL_), "bottom left" );
        }
        else if (marker == 2) { // bottom right
            printPosition( recognizePosition(_pathBR_), "bottom right");
        }
        else if (marker == 3) { // upper left
            printPosition( recognizePosition(_pathUL_), "upper left");
        }
        else if (marker == 4) { // upper right
            printPosition( recognizePosition(_pathUR_), "upper right");
        }

        if (modeWrite == true)
            video.write(outframe);
        imshow("main", outframe);
        if(waitKey(90) >= 0) break;
    }
    return 0;
}
