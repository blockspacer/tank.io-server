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
    p-=pos;
    p=p.rotate(mozdavaj);
    if(-h_d<p.x && p.x<h_d)
        return abs(p.y);

    if(p.x>0)
        p.x-=h_d;
    else
        p.x+=h_d;
    return p.length();
}

float SegmentLine::have_intersec(Point s,Point f,float r){
    Point s_p=s-pos;
    s_p=s_p.rotate(mozdavaj);

    Point f_p=f-pos;
    f_p=f_p.rotate(mozdavaj);

    if((f_p.y <-0.00001 && s_p.y>0.00001)||
            (f_p.y >0.00001 && s_p.y<-0.00001)){
        float mz_x=(f_p.x*abs(s_p.y)+s_p.x*abs(f_p.y))/
                (abs(f_p.y)+abs(s_p.y));
        if(abs(mz_x)<h_d)
            return true;
    }
    if(abs(s_p.x)<h_d && abs(s_p.y)<r){
        return true;
    }
    if(abs(f_p.x)<h_d && abs(f_p.y)<r){
        return true;
    }
    return false;

}
