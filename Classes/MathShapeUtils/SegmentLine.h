#ifndef SEGMENTLINE_H
#define SEGMENTLINE_H

#include "cocos2d_server/cocos2d_server.h"

struct SegmentLine
{
    Point start,finish;
    SegmentLine();


    float length()const;

    void shif(const Point &p);
    void rotate(const Point &rotate_vector);
    void rotate(const Point &center,const Point &rotate_vector);

    bool have_intersec(const SegmentLine &s,Point &out)const;
    bool have_intersec(const Point &center,const SegmentLine &s)const;
};

#endif // SEGMENTLINE_H
