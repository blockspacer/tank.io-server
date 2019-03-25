#include "SegmentLine.h"

SegmentLine::SegmentLine()
{

}
float SegmentLine::length()const{
    return (finish-start).length();
}

void SegmentLine::refresh(){
    h_d=length()/2;
    Point _p=finish-start;
    _p.normalize();

    mozdavaj={_p.x,-_p.y};
    direction=_p;
    amud={_p.y,-_p.x};

    //angle=Base2D::find_angle<Point>(_p);
}

void SegmentLine::shif(const Point &p){
    pos+=p;
}
void SegmentLine::rotate(const Point &rotate_vector){//TODO optimize this function
    pos=pos.rotate(rotate_vector);//che bahal
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

float SegmentLine::get_distance(Point p){
    p-pos;
    p=p.rotate(mozdavaj);
    if(p.x<h_d && p.x>-h_d)
        return p.y;
    //TODO not optimum
    float r1=(p-start).length();
    float r2=(p-finish).length();
    return r1<r2 ? r1 : r2;
}

float SegmentLine::have_intersec(Point s,Point f,float r){
    Point p=s-pos;
    Point mz=p.rotate(mozdavaj);

    Point prv_p=f-pos;
    Point prv_mz=prv_p.rotate(mozdavaj);

    if((prv_mz.y <-0.00001 && mz.y>0.00001)||
            (prv_mz.y >0.00001 && mz.y<-0.00001)){
        float mz_x=(prv_mz.x*abs(mz.y)+mz.x*abs(prv_mz.y))/
                (abs(prv_mz.y)+abs(mz.y));
        if(abs(mz_x)<h_d)
            return true;
    }
    if(abs(mz.x)<h_d && abs(mz.y)<r){
        float f=r-abs(mz.y);
        return true;
    }
    return false;

}
