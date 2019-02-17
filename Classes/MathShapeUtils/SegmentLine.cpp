#include "SegmentLine.h"

SegmentLine::SegmentLine()
{

}
float SegmentLine::length()const{
    return (finish-start).length();
}
void SegmentLine::shif(const Point &p){
    start+=p;
    finish+=p;
}
void SegmentLine::rotate(const Point &rotate_vector){//TODO optimize this function
    start=start.rotate(rotate_vector);
    finish=finish.rotate(rotate_vector);
}

void SegmentLine::rotate(const Point &center,const Point &rotate_vector){//TODO optimize this function
    start-=center;
    finish-=center;
    start.rotate(rotate_vector);
    finish.rotate(rotate_vector);
    start+=center;
    finish+=center;
}

bool SegmentLine::have_intersec(const SegmentLine &s,Point &out)const{
    /* shif segments to start
     * unrotate segments  with normilize of finish
     *
     *
    */

    SegmentLine tmp=s;
    SegmentLine thiz=*this;
    Point shift_verctor=-start;

    thiz.shif(shift_verctor);
    tmp.shif(shift_verctor);

    Point rotate_verctor=thiz.finish;
    tmp.rotate(rotate_verctor);//TODO unrotate
    thiz.rotate(rotate_verctor);//TODO unrotate

    //Point res;
    out.y=0;
    out.x=tmp.start.x+(tmp.finish.x-tmp.start.x)*tmp.finish.y/tmp.length(); ;

    out.rotate(rotate_verctor);
    out-=shift_verctor;

    return  false;
}
