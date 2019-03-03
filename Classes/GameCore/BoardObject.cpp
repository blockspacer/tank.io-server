#include "BoardObject.h"
#include <stdio.h>
#include <string.h>

#include "GameCore.h"

#include <iostream>
using namespace std;
#include "AbstractTankViewHandler.h"
BoardObject::BoardObject(GameCore *core):core(core){
    my_step=core->total_time;
}
void BoardObject::remove(){
    if(view!=nullptr)
        view->on_ded();
}
void BoardObject::get_data(BoardObjectState *state)const{
    state->object_id=id;
    state->pos=ZIP_Point::fcreate(pos.x,pos.y);
    state->z=z;
    //cout<<"angle "<<angle<<" "<<int(angle*1000)<<endl;
    state->angle=angle;
    state->room_time=core->total_time;
    state->r=r;


}
MyDataBlock BoardObject::get_data()const{
    CCLOG("this faunction not be for call");
}
void BoardObject::set_data(const BoardObjectState *state){
    id=state->object_id;
    pos=state->pos.get();
    z=state->z;
    angle=state->angle;
    r=state->r;
    last_update_from_server_time=state->room_time;
    my_step=last_update_from_server_time;
}

void LiveBoardObject::update(){
    float dt=core->step_time;
    v=Point(cos(angle),sin(angle))*speed;
    _next_pos=pos+v*dt;
}
void LiveBoardObject::get_data(BoardObjectState *state)const{
    BoardObject::get_data(state);
    auto cstate=static_cast<LiveBoardObjectState*>(state);
    cstate->speed=speed;
    cstate->healt=healt;
    cstate->max_healt=max_healt;


}
MyDataBlock LiveBoardObject::get_data()const{
    auto data=new LiveBoardObjectState();
    MyDataBlock res;
    get_data(data);
    res.data = (char*)data;
    res.size = sizeof(LiveBoardObjectState);
    return res;
}
void LiveBoardObject::set_data(const BoardObjectState *state){
    BoardObject::set_data(state);
    auto cstate=static_cast<const LiveBoardObjectState*>(state);
    speed=cstate->speed;
    healt=cstate->healt;
    max_healt=cstate->max_healt;

}

