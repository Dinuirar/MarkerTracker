#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;

int main( ) {
    VideoCapture cap(0);
    if(!cap.isOpened())
        return -1;

    int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
    VideoWriter video("../outA.avi",
                      CV_FOURCC('M','J','P','G'),
                      10,
                      Size( frame_width,frame_height ),
                      true);

    namedWindow("main",1);
    for(;;) {
        Mat frame;
        cap >> frame; // get a new frame from camera
        video.write(frame);
        imshow("main", frame);
        if(waitKey(30) >= 0) break;
    }
    return 0;
}
