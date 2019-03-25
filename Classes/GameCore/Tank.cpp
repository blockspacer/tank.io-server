#include "Tank.h"
#include "GameCore.h"

#include "AbstractTankViewHandler.h"
#include "PlatformLine.h"
#include "Misle.h"
#include "MathShapeUtils/Base.h"

void Unit::clear_collision(){

    Point force={0,0};
    float XS[1024],YS[1024];
    int x_n=0,y_n=0;
    Point SF={0,0};
    bool ph=false;
    //for(CirclePartShape &part :parts)
    {
        //part.sum_of_force={0,0};
        //part.global_pos=pos+part._pos;
        for(const PlatformLine *line: core->lines){
            Point p=_next_pos-line->pos;

            Point mz=p.rotate(line->l.mozdavaj);

            if(abs(mz.x)<line->l.h_d && abs(mz.y)<r){
                float f=r-abs(mz.y);
                if(mz.y<0)
                    f*=-1;
                float upc=line->l.amud.dot(v);
                float emc=line->l.direction.dot(v);
                force = -line->l.amud*upc;
                force+= line->l.direction*emc;
                SF+=force;
                ph=true;
                if(mz.y>0)
                    _next_pos+=-line->l.amud*(r-(mz.y));
                else
                    _next_pos+=line->l.amud*(r+(mz.y));

                //part.sum_of_force+=force;
                //force.normalize();
                //v=force*speed; reflect object

                if(view!=nullptr)
                    static_cast<AbstractTankViewHandler*>(view)->show_cross_with_block({0,0},0);
                //return;
            }
        }
    }
    if(ph){
        SF.normalize();
        //pos=prv_pos;
    }
    /*float fx=sum(XS,x_n);
    float fy=sum(YS,y_n);
    force+={fx,fy};
    __pos2=__pos+force;/**/

    for(auto block:core->blocks)if(block){
        float R=this->r+block->r;
        Point p=_next_pos-block->pos;
        if(p.length()<R){
            p.normalize();
            p*=R;
            _next_pos=block->pos+p;
        }
    }
    /*for(auto tank_ptr:core->tanks)if(tank_ptr.second && tank_ptr.second->id!=owner_id){
        auto tank=tank_ptr.second;
        float R=r+tank->r;
        Point p=tank->pos-pos;
        float d=p.length();
        if(d<R){
           for_remove=true;
            tank->get_damage(10);
           if(view)
               view->on_ded();
           view=nullptr;
        }
    }/**/
    float max_d=core->max_d;
    if(_next_pos.length()>max_d-r){
        _next_pos.normalize();
        _next_pos*=(max_d-r);
    }

}
void Unit::get_damage(int x){
    healt-=x;
    if(core->server)
        last_event_time=core->total_time;

}
void Unit::get_data(BoardObjectState *s)const{
    LiveBoardObject::get_data(s);
    s->object_type=BoardObjectState::ObjectType::TANK;
    auto cs=static_cast<UnitState*>(s);
    cs->target_angle=target_angle;
    cs->user_id=user_id;
    cs->angle_speed=angle_speed;
}
MyDataBlock Unit::get_data()const{
    auto data=new UnitState();
    MyDataBlock res;
    get_data(data);
    res.data = (char*)data;
    res.size = sizeof(UnitState);
    return res;
}


void Unit::set_data(const BoardObjectState *state){
    if(last_update_from_server_time>state->room_time)
        return;
    LiveBoardObject::set_data(state);
    auto cs=static_cast<const UnitState*>(state);
    target_angle=cs->target_angle;
    user_id=cs->user_id;
    angle_speed=cs->angle_speed;


}


Tank::Tank(GameCore *core):Unit(core){
    max_angle_speed=0.5*3.1415*10;
    r=50;
    CirclePartShape p;
    p.r=5;
    p._pos={0,80};
    parts.push_back(p);
    p._pos={0,-80};
    parts.push_back(p);

    p._pos={80,0};
    parts.push_back(p);
    p._pos={-80,0};
    parts.push_back(p);
    target_range=1000;

    max_shot=3;
    shot_damage=30;
}
void Tank::update_inputs(){
    if(ai!=nullptr)
        ai->step();
    if(speed>max_speed)
        speed=max_speed;
    if(speed<0)
        speed=0;
    while(inps.size()){
        auto i=inps.begin();
        if(i->second!=nullptr && i->second->trigger_step >= my_step)
            break;
        inps.erase(i);
    }
    while(cinps.size()){
        auto i=cinps.begin();
        auto t=i->second;
        if(i->second!=nullptr && i->second->trigger_step >= my_step-1000)
            break;
        cinps.erase(i);
    }
    while(sinps.size()){
        auto i=sinps.begin();
        auto t=i->second;
        if(i->second!=nullptr && i->second->trigger_step >= my_step)
            break;
        sinps.erase(i);
    }
    while(inps.find(my_step)!=inps.end()){
        auto i=inps.find(my_step);
        if(i->second->trigger_step==my_step){
            i->second->run_on_this(this);
            core->view_handler->triger_change(id);
            if(core->server)
                last_event_time=core->total_time;
        }
        //cinps.erase(i->second->client_id);
        inps.erase(i);


    }
}
void Tank::update_angle(){
    float dt=core->step_time;


    target_angle=GameCore::angle_refresh_pi_npi(target_angle);
    angle=GameCore::angle_refresh_pi_npi(angle);
    float dis_angle=GameCore::angle_refresh_pi_npi(target_angle-angle);

    {
        angle_speed=max_angle_speed*dis_angle*10;
        if(angle_speed>max_angle_speed)
            angle_speed=max_angle_speed;
        if(angle_speed<-max_angle_speed)
            angle_speed=-max_angle_speed;/**/
    }


    float d_a=dt*angle_speed;
    if(dis_angle<-d_a ){
        angle+=d_a;
    }if(dis_angle>d_a){
        angle+=d_a;
    }else if(abs(dis_angle)<d_a){


        angle=target_angle;
    }/**/
    angle=target_angle;



    angle=int(angle*1000)*0.001;

}
void Tank::update(){
    update_inputs();
    if(my_step>=core->total_time){//TODO set tag dont check collision
        _next_pos=pos;
        return;
    }


    update_angle();

    remain_shot+=core->step_time*1.5;
    if(remain_shot>max_shot)
        remain_shot=max_shot;

    ++my_step;
    history[my_step]=PostionConfig{pos=pos,z=z};
    history.erase(my_step-30*3);

    LiveBoardObject::update();









}

Misle *Tank::shot(OBJ_ID shot_id,bool targeted,float angle2){
    CCLOG("Tank::shot %lld",shot_id);
    if(remain_shot<1)
        return nullptr;
    remain_shot-=1;

    Misle *t=core->create_shot();
    t->damage=shot_damage;
    t->id=shot_id;
    t->owner_id=id;
    if(!targeted){
        auto o_id=sugesst_target();
        if(o_id!=0){
            Point p=core->all_objects[o_id]->pos-pos;
            angle2=Base2D::find_angle<Point>(p);
            canon_angle=angle2;
        }else
            angle2=canon_angle;
    }
    t->pos=pos+Point(cos(angle2),sin(angle2))*100;
    t->_next_pos=t->pos;
    t->angle=angle2;
    t->speed=2500;
    t->z=50;
    t->max_dis=0.8;
    core->init_shot(t);
    if(core->server)
        last_event_time=core->total_time;
    return t;
}
Tank::InputFarman* Tank::add_action(STEP_VALUE action_time,float f,float s,int client_id){
     CCLOG("shot_action %d \n",client_id);
    auto p=new InputFarman();
    Input::ptr i=Input::ptr(p);
    p->trigger_step=action_time;
    p->client_id=client_id;
    p->target_farman=f;
    p->target_speed=s;
    p->id=action_time;//TODO bayad dorst beshe
    inps.insert(std::pair<STEP_VALUE,Input::ptr>(p->trigger_step,i));
    cinps[client_id]=i;
    //view_handler->show_shot_animation()
    return p;
}
void Tank::Input::get_data(State *data)const{
    auto res=static_cast<State*>(data);
    res->trigger_step=trigger_step;
    res->id=id;
    res->client_id=client_id;
    res->tank_id=id;
    data->size_of_data=sizeof (Tank::Input::State);
}
void Tank::Input::set_data(const State *data){
    trigger_step=data->trigger_step;
    id=data->id;
}
void Tank::InputFarman::get_data(Input::State *data)const{
    Input::get_data(data);
    auto res=static_cast<InputFarman::State*>(data);
    res->farman=target_farman;
    res->speed=target_speed;
    res->type=State::Type::MOVMENT;


    data->size_of_data=sizeof (Tank::InputFarman::State);


}
void Tank::InputFarman::set_data(const Input::State *data){
    Input::set_data(data);
    auto res=static_cast<const State*>(data);
    target_farman=res->farman;
    target_speed=res->speed;
}

void Tank::InputShot::run_on_this(Tank *thiz)const{
    if(thiz->core->server)
        Misle *shot=thiz->shot(shot_id,targetet,angle);
}
void Tank::InputShot::get_data(Input::State *data)const{
    Input::get_data(data);
    auto res=static_cast<State*>(data);
    res->shot_id=shot_id;
    res->type=State::Type::SHOT;
    res->targetet=targetet;
    res->angle=angle;
    data->size_of_data=sizeof (Tank::InputShot::State);
}
void Tank::InputShot::set_data(const Input::State *data){
    Input::set_data(data);
    auto res=static_cast<const State*>(data);
    shot_id=res->shot_id;
    targetet=res->targetet;
    angle=res->angle;
}

Tank::InputShot* Tank::add_shot_action(STEP_VALUE action_time,int client_id){
    CCLOG("add_shot_action %d \n",client_id);
    auto p=new InputShot();
    Input::ptr i=Input::ptr(p);
    p->trigger_step=action_time;
    p->client_id=client_id;
    p->id=action_time;//TODO bayad dorst beshe
    p->shot_id=core->generate_shot_id();
    inps.insert(std::pair<STEP_VALUE,Input::ptr>(p->trigger_step,i));
    cinps[client_id]=i;
    sinps[p->id]=i;
    return p;
}

OBJ_ID Tank::sugesst_target(){
    float best_target=target_range;
    OBJ_ID res=0;
    for(auto t : core->tanks)//TODO check TILE
        if(t.second!=nullptr &&
           !t.second->for_remove &&
            t.second!=this &&
            (t.second->team_index!=team_index || team_index==0)){
            Point p=t.second->pos;
            p-=pos;
            if(p.length()<best_target){
                best_target=p.length();
                res=t.first;
            }
        }
    return res;
}
Tank::Input* Tank::get_action(int client_id){
    if(cinps.find(client_id)==cinps.end())
        return nullptr;
    return cinps[client_id].get();
}
void Tank::add_created_action(Input *inp){
    if(sinps.find(inp->id)!=sinps.end())
        return;
    Input::ptr i=Input::ptr(inp);
    inps.insert(std::pair<STEP_VALUE,Input::ptr>(inp->trigger_step,i));
    CCLOG("add_created_action %d",inp->id);
    sinps[inp->id]=i;
}


void Tank::show_shot_animation(STEP_VALUE shot_time){
    if(view)
        static_cast<AbstractTankViewHandler*>(view)->show_shot_animation((shot_time-core->total_time)*core->step_time);
}

void Tank::get_data(BoardObjectState *s)const{
    Unit::get_data(s);
    s->object_type=BoardObjectState::ObjectType::TANK;
    auto cs=static_cast<TankState*>(s);
    cs->canon_angle=canon_angle;
    cs->target_range=target_range;
    cs->shot_damage=shot_damage;
    cs->remain_shot=remain_shot;
}
MyDataBlock Tank::get_data()const{
    auto data=new TankState();
    MyDataBlock res;
    get_data(data);
    res.data = (char*)data;
    res.size = sizeof(TankState);
    return res;
}
void Tank::set_data(const BoardObjectState *state){
    if(last_update_from_server_time>state->room_time)
        return;
    Unit::set_data(state);
    auto cs=static_cast<const TankState*>(state);
    canon_angle=cs->canon_angle;
    target_range=cs->target_range;
    shot_damage=cs->shot_damage;
    remain_shot=cs->remain_shot;
}




void CircleBlock::get_data(BoardObjectState *s)const{
    BoardObject::get_data(s);
    s->object_type=BoardObjectState::ObjectType::CircleBlock;
}
MyDataBlock CircleBlock::get_data()const{
    auto data=new CircleBlockState();
    MyDataBlock res;
    get_data(data);
    res.data = (char*)data;
    res.size = sizeof(CircleBlockState);
    return res;
}

void CircleBlock::set_data(const BoardObjectState *state){
    if(last_update_from_server_time>state->room_time)
        return;
    BoardObject::set_data(state);
}


