#include "PlatformLine.h"
#include "MathShapeUtils/Base.h"


void PlatformLine::get_data(BoardObjectState *s)const{
    BoardObject::get_data(s);
    s->object_type=BoardObjectState::ObjectType::LineBlock;
    auto cstate=static_cast<PlatformLineState*>(s);
    cstate->start=ZIP_Point::fcreate(l.start);
    cstate->finish=ZIP_Point::fcreate(l.finish);
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
    l.start=cstate->start.get();
    l.finish=cstate->finish.get();
    refresh();
}

/*PlatformLine::PlatformLine(GameCore *gc,Point pos,Point _direction,float h_d):BoardObject(gc){
    this->h_d=h_d;
    this->pos=pos;
    l.pos=pos;

    _direction.normalize();
    mozdavaj={_direction.x,-_direction.y};
    direction=_direction;
    amud={_direction.y,-_direction.x};
}/**/
PlatformLine::PlatformLine(GameCore *gc,Point s,Point f):BoardObject(gc){
    pos=(s+f)/2;
    l.pos=pos;
    l.start=s;
    l.finish=f;
    l.start-=pos;
    l.finish-=pos;
    refresh();
}
void PlatformLine::refresh(){
    l.refresh();

    angle=Base2D::find_angle<Point>(l.direction);
}
