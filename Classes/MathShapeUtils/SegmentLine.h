#ifndef SEGMENTLINE_H
#define SEGMENTLINE_H

#include "cocos2d_server/cocos2d_server.h"
struct BaseSegmentLine{

};
struct SegmentLine
{
    Point pos;
    Point start,finish;

    Point mozdavaj;
    Point direction;
    Point amud;
    float h_d;

    SegmentLine();
    SegmentLine(const Point &s,const Point &f){
        pos=(s+f)/2;
        start=s-pos;
        finish=f-pos;
        refresh();
    }


    float length()const;

    void refresh();

    void shif(const Point &p);
    void rotate(const Point &rotate_vector);
    void rotate(const Point &center,const Point &rotate_vector);

    bool have_intersec(const SegmentLine &s,Point &out)const;
    bool have_intersec(const Point &center,const SegmentLine &s)const;

    float get_distance(Point p);

    float have_intersec(Point s,Point f,float r);

};

#endif // SEGMENTLINE_H
