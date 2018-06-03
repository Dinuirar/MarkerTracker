#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <QtGlobal>
#include <cmath>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;

extern float diff_length_thresh;
extern float diff_area_thresh;
extern u_int NUMBER_OF_CIRCLES;
enum ECircle { BL, BR, UL, UR, N };
enum EPosition { POS_UP, POS_DOWN, POS_LEFT, POS_RIGHT, POS_CENTER, POS_UNKNOWN };

struct path_el {
    Point point;
    u_int frame;
};

typedef std::vector<path_el> PathType;
//typedef std::vector<Point> PathType;
typedef std::vector<Point> PointsType;

// determine wheter contour is a marker or not
bool validateCircle(float p_ref_radius, const vector<Point>& p_contour);
void drawPath(PathType _p, Scalar _color, Mat& frame);
qreal min( qreal dists[], u_int size );
qreal distance(Point A, Point B);
ECircle findCircle(Point point,
                   PathType _pBL,
                   PathType _pBR,
                   PathType _pUL,
                   PathType _pUR);

int compare (const void* p1, const void* p2);
void filterPath(PathType& _p);
EPosition recognizePosition(const PathType& _p);
void printPosition(EPosition, string);

#endif // FUNCTIONS_H
