#include "PlatformLine.h"
#include "MathShapeUtils/Base.h"


void PlatformLine::get_data(BoardObjectState *s)const{
    BoardObject::get_data(s);
    s->object_type=BoardObjectState::ObjectType::LineBlock;
    auto cstate=static_cast<PlatformLineState*>(s);
    cstate->start=ZIP_Point::fcreate(start);
    cstate->finish=ZIP_Point::fcreate(finish);
}
MyDataBlock PlatformLine::get_data()const{
    auto data=new PlatformLineState();
    MyDataBlock res;
    get_data(data);
    res.data = (char*)data;
    res.size = sizeof(PlatformLineState);
    return res;
}

void PlatformLine::set_data(const BoardObjectState *state){
    if(last_update_from_server_time>state->room_time)
        return;
    BoardObject::set_data(state);
    auto cstate=static_cast<const PlatformLineState*>(state);
    start=cstate->start.get();
    finish=cstate->finish.get();
    refresh();
}

PlatformLine::PlatformLine(GameCore *gc,Point pos,Point _p,float h_d):BoardObject(gc){
    this->h_d=h_d;
    this->pos=pos;
    _p.normalize();
    mozdavaj={_p.x,-_p.y};
    direction=_p;
    amud={_p.y,-_p.x};
}
PlatformLine::PlatformLine(GameCore *gc,Point s,Point f):BoardObject(gc){
    pos=(s+f)/2;
    start=s;
    finish=f;
    start-=pos;
    finish-=pos;
    refresh();
}
void PlatformLine::refresh(){
    h_d=(start-finish).length()/2;
    Point _p=finish-start;
    _p.normalize();

    mozdavaj={_p.x,-_p.y};
    direction=_p;
    amud={_p.y,-_p.x};

    angle=Base2D::find_angle<Point>(_p);
}
