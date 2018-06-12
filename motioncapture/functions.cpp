#include "functions.h"

int markersX1 = 180;
int markersX2 = 380;
int markersY1 = 217;
int markersY2 = 317;

float diff_length_thresh = 100;
float diff_area_thresh = 100;
float max_radius_thresh = 10;
float min_radius_thresh = 2;
u_int NUMBER_OF_CIRCLES = 4;

// determine whether contour is a marker or not
bool validateCircle(float p_ref_radius, const vector<Point>& p_contour) {
    if( !(p_ref_radius < max_radius_thresh && p_ref_radius > min_radius_thresh) )
        return false;
//    return true;

    float ref_length = 2 * M_PI * p_ref_radius;
    float real_length = arcLength(p_contour, true);
    float ref_area = M_PI * p_ref_radius * p_ref_radius;
    float real_area = contourArea(p_contour);

    if ( abs(ref_length - real_length) < diff_length_thresh
         && abs(ref_area - real_area) < diff_area_thresh )
        return true;
    else
        return false;
}

void drawPath(PathType _p, Scalar _color, Mat& frame) {
    if( _p.size() < 1 )
        return;
    for (u_int i = 0; i < _p.size()-1; i++) {
        circle(frame, _p[i].point, 1, _color, 1, 1, 0);
        line(frame, _p[i].point, _p[i+1].point, _color);
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
        distBL = distance(point, _pBL[ _pBL.size()-1 ].point );
    else distBL = MAXDIST;

    if( _pBR.size() != 0 )
        distBR = distance(point, _pBR[ _pBR.size()-1 ].point);
    else distBR = MAXDIST;

    if( _pUR.size() != 0 )
        distUR = distance(point, _pUR[ _pUR.size()-1 ].point);
    else distUR = MAXDIST;

    if( _pUL.size() != 0 )
        distUL = distance(point, _pUL[ _pUL.size()-1 ].point);
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
    return 0;
}

void filterPath(PathType& _p) {
    u_int lastFrame = _p[ _p.size()-1 ].frame;
    if( lastFrame < 3 )
        return;
    if( lastFrame == _p.size() )
        return;
    vector<path_el>::iterator it = _p.begin();
    path_el tmp;
    // search for missing points
    for (u_int i = 2; i <= lastFrame; i++) {
        if(_p[i-1].frame != i) { // if there is an empty point in the path...
            tmp.frame = i; // assign missing frame number
            tmp.point = _p[i-1].point;
            it = _p.insert(it, tmp);
        }
        it++;
    }
    return;
}

Point calcMeanPoint(const PathType& _p) {
    u_int HOWMANY = 5;
    if ( HOWMANY <= _p.size() )
        HOWMANY = _p.size()-1;
    int xs = 0;
    int ys = 0;
    Point mean;
    for(u_int i = 0; i <= HOWMANY; i++) {
        xs += _p[i].point.x;
        ys += _p[i].point.y;
    }
    mean.x = xs / HOWMANY;
    mean.y = ys / HOWMANY;
    return mean;
}

EPosition recognizePosition(const PathType& _p) {
    Point centerpoint = calcMeanPoint(_p);
    Point lastpoint = _p[ _p.size()-1 ].point;
    Point difference = lastpoint - centerpoint;
    int eps = 15;
    if( distance(centerpoint, lastpoint) < eps) {
        return POS_CENTER;
    }
    else if( difference.x < -eps && abs(difference.y) < eps ) {
        return POS_LEFT;
    }
    else if( abs(difference.x) < eps && difference.y < -eps ) {
        return POS_UP;
    }
    else if( abs(difference.x) < eps && difference.y > eps ) {
        return POS_DOWN;
    }
    else if( difference.x > eps && abs(difference.y) < eps ) {
        return POS_RIGHT;
    }
    else {
        return POS_UNKNOWN;
    }
}

void printPosition(EPosition position, string markername) {
    string positionname;
    if (position == POS_CENTER)
        positionname = "center";
    else if (position == POS_RIGHT)
        positionname = "right";
    else if (position == POS_LEFT)
        positionname = "left";
    else if (position == POS_DOWN)
        positionname = "down";
    else if (position == POS_UP)
        positionname = "up";
    else
        positionname = "unknown";
    cout << markername << " is " << positionname << "\n";
}
