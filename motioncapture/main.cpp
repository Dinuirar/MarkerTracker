#include <iostream>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;

float diff_length_thresh = 100;
float diff_area_thresh = 100;
float min_radius_thresh = 10;

// determine wheter contour is a marker or not
bool validateCircle(float p_ref_radius, const vector<Point>& p_contour) {
    float ref_length = 2 * M_PI * p_ref_radius;
    float real_length = arcLength(p_contour, true);

    float ref_area = M_PI * p_ref_radius * p_ref_radius;
    float real_area = contourArea(p_contour);

    if ( abs(ref_length - real_length) < diff_length_thresh
         && abs(ref_area - real_area) < diff_area_thresh
         && p_ref_radius < min_radius_thresh)
        return true;
    else
        return false;
}

int main( ) {
//    VideoCapture cap(0);
    string path = "/home/kassus/Desktop/PROJEKTY/projektWMA/machine-vision/outA.mkv";
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
//    for(;;) {
    while( cap.read(frame) ) {
//        cap >> frame; // get a new frame from camera
        cvtColor(frame, frame, CV_BGR2GRAY);
//        GaussianBlur( frame, frame, Size(3, 3), 2, 2 );
        // or ADAPTIVE_THRESH_MEAN/GAUSSIAN_C
        //adaptiveThreshold(frame, frame, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 17, 0);
        threshold(frame, frame, 160, 255, THRESH_BINARY);

        erode(frame, frame, Mat(), Point(-1, -1), 3);
        dilate(frame, frame, Mat(), Point(-1, -1), 3);

        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        findContours( frame, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
//        for( int i = 0; i< contours.size(); i++ ) {
//             Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
//             drawContours( frame, contours, i, color, 2, 8, hierarchy, 0, Point() );
//        }

        cvtColor(frame, frame, CV_GRAY2BGR);
        Point2f* centers = new Point2f[ contours.size() ];
        float* radiuses = new float[ contours.size() ];
        for (int i = 0; i < contours.size(); i++) {
            minEnclosingCircle(contours[i], centers[i], radiuses[i]);
            if ( validateCircle( radiuses[i], contours[i] ) )
                circle(frame, centers[i], radiuses[i], Scalar(255, 0, 0), 3, 8, 0);
        }

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
        if(waitKey(60) >= 0) break;
    }
    return 0;
}
