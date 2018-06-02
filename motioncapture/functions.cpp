#include "functions.h"

float diff_length_thresh = 80;
float diff_area_thresh = 80;
float min_radius_thresh = 10;
u_int NUMBER_OF_CIRCLES = 4;

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

void drawPath(PathType _p, Scalar _color, Mat& frame) {
    if( _p.size() < 1 )
        return;
    for (u_int i = 0; i < _p.size()-1; i++) {
        circle(frame, _p[i], 1, _color, 1, 1, 0);
//        line(frame, _p[i], _p[i+1], _color);
    }
}

qreal min( qreal dists[], u_int size ) {
    qreal min = dists[0];
    for (u_int i = 1; i < size; i++) {
        if ( dists[i] < min )
            min = dists[i];
    }
    return min;
}

qreal distance(Point A, Point B) {
    return norm( A - B );
}

ECircle findCircle(Point point, PathType _pBL, PathType _pBR, PathType _pUL, PathType _pUR) {
    qreal MAXDIST = 1000;
    qreal distBL,
            distBR,
            distUL,
            distUR;

    if( _pBL.size() != 0 )
        distBL = distance(point, _pBL[ _pBL.size()-1 ] );
    else distBL = MAXDIST;

    if( _pBR.size() != 0 )
        distBR = distance(point, _pBR[ _pBR.size()-1 ]);
    else distBR = MAXDIST;

    if( _pUR.size() != 0 )
        distUR = distance(point, _pUR[ _pUR.size()-1 ]);
    else distUR = MAXDIST;

    if( _pUL.size() != 0 )
        distUL = distance(point, _pUL[ _pUL.size()-1 ]);
    else distUL = MAXDIST;

    qreal dists[4];
    dists[0] = distBL;
    dists[1] = distBR;
    dists[2] = distUL;
    dists[3] = distUR;

    qreal distMin = min(dists, 4);
    if( distMin == distUR )
        return UR;
    else if( distMin == distBR )
        return BR;
    else if( distMin == distBL )
        return BL;
    else if( distMin == distUL )
        return UL;
    return N;
}

int compare (const void *p1, const void *p2) {
    if( *(int*)p1 < *(int*)p2 ) return -1;
    if( *(int*)p1 == *(int*)p2 ) return 0;
    if( *(int*)p1 > *(int*)p2 ) return 1;
}
