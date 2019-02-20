#include "Tank.h"
#include "GameCore.h"

#include "AbstractTankViewHandler.h"
#include "PlatformLine.h"
#include "Misle.h"

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
            Point mz=p.rotate(line->mozdavaj);

            if(abs(mz.x)<line->h_d && abs(mz.y)<r){
                float f=r-abs(mz.y);
                if(mz.y<0)
                    f*=-1;
                float upc=line->amud.dot(v);
                float emc=line->direction.dot(v);
                force = -line->amud*upc;
                force+= line->direction*emc;
                SF+=force;
                ph=true;
                if(mz.y>0)
                    _next_pos+=-line->amud*(r-(mz.y));
                else
                    _next_pos+=line->amud*(r+(mz.y));

                //part.sum_of_force+=force;
                //force.normalize();
                //v=force*speed; reflect object

                if(view_handler!=nullptr)
                    view_handler->show_cross_with_block({0,0},0);
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

void Unit::get_data(BoardObjectState *s)const{
    LiveBoardObject::get_data(s);
    s->object_type=BoardObjectState::ObjectType::TANK;
    auto cs=static_cast<UnitState*>(s);
    cs->target_angle=target_angle;
    cs->user_id=user_id;
    cs->angle_speed=angle_speed;
    s->size_of_data=sizeof(UnitState);
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
    max_angle_speed=0.5*3.1415;
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

}
void Tank::update_inputs(){
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



    angle=int(angle*1000)*0.001;

}
void Tank::update(){
    update_inputs();
    if(my_step>=core->total_time){//TODO set tag dont check collision
        _next_pos=pos;
        return;
    }


    update_angle();

    ++my_step;
    history[my_step]=PostionConfig{pos=pos,z=z};
    history.erase(my_step-30*3);

    LiveBoardObject::update();









}

Misle *Tank::shot(OBJ_ID shot_id){
    CCLOG("Tank::shot %lld",shot_id);
    Misle *t=core->create_shot();
    t->id=shot_id;
    t->owner_id=id;
    t->pos=pos+Point(cos(angle),sin(angle))*80;
    t->_next_pos=pos;
    t->angle=angle;
    t->speed=2*800;
    t->z=50;
    core->init_shot(t);
    return t;
}
Tank::InputFarman* Tank::add_action(STEP_VALUE action_time,float f,int client_id){
     CCLOG("shot_action %d \n",client_id);
    auto p=new InputFarman();
    Input::ptr i=Input::ptr(p);
    p->trigger_step=action_time;
    p->client_id=client_id;
    p->target_farman=f;
    p->id=action_time;//TODO bayad dorst beshe
    inps.insert(std::pair<STEP_VALUE,Input::ptr>(p->trigger_step,i));
    cinps[client_id]=i;
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
    res->type=State::Type::MOVMENT;


    data->size_of_data=sizeof (Tank::InputFarman::State);


}
void Tank::InputFarman::set_data(const Input::State *data){
    Input::set_data(data);
    auto res=static_cast<const State*>(data);
    target_farman=res->farman;
}

void Tank::InputShot::run_on_this(Tank *thiz)const{
    if(thiz->core->server)
        Misle *shot=thiz->shot(shot_id);
}
void Tank::InputShot::get_data(Input::State *data)const{
    Input::get_data(data);
    auto res=static_cast<State*>(data);
    res->shot_id=shot_id;
    res->type=State::Type::SHOT;
    data->size_of_data=sizeof (Tank::InputShot::State);
}
void Tank::InputShot::set_data(const Input::State *data){
    Input::set_data(data);
    auto res=static_cast<const State*>(data);
    shot_id=res->shot_id;
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
    if(view_handler)
        view_handler->show_shot_animation((shot_time-core->total_time)*core->step_time);
}

void Tank::get_data(BoardObjectState *s)const{
    Unit::get_data(s);
    s->object_type=BoardObjectState::ObjectType::TANK;
    auto cs=static_cast<TankState*>(s);
    s->size_of_data=sizeof(TankState);

}

void Tank::set_data(const BoardObjectState *state){
    if(last_update_from_server_time>state->room_time)
        return;
    Unit::set_data(state);


}




void CircleBlock::get_data(BoardObjectState *s)const{
    BoardObject::get_data(s);
    s->object_type=BoardObjectState::ObjectType::CircleBlock;
    s->size_of_data=sizeof(CircleBlockState);
}

void CircleBlock::set_data(const BoardObjectState *state){
    if(last_update_from_server_time>state->room_time)
        return;
    BoardObject::set_data(state);
}


