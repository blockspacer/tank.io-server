#include "Misle.h"
#include "PlatformLine.h"
#include "Tank.h"
#include "AbstractTankViewHandler.h"
#include "GameCore.h"



void Misle::get_data(BoardObjectState *s)const{
    LiveBoardObject::get_data(s);
    s->object_type=BoardObjectState::ObjectType::MISLE;
    s->owner_id=owner_id;
    MisleState *ss=static_cast<MisleState*>(s);
    ss->max_dis=max_dis;
    ss->damage=damage;
}
MyDataBlock Misle::get_data()const{
    auto data=new MisleState();
    MyDataBlock res;
    get_data(data);
    res.data = (char*)data;
    res.size = sizeof(MisleState);
    return res;
}
void Misle::set_data(const BoardObjectState *state){
    if(last_update_from_server_time>state->room_time)
        return;
    LiveBoardObject::set_data(state);
    //speed=state->speed;
    owner_id=state->owner_id;
    auto *ss=static_cast<const MisleState*>(state);
    max_dis=ss->max_dis;
    damage=ss->damage;
}

void Misle::update(){
    float t=core->step_time*(core->total_time-born_step);
    if(t>max_dis){
        miss();
    }
    LiveBoardObject::update();
}
void Misle::miss(){
    for_remove=true;
    dead();
}

void Misle::clear_collision(){

    Point force={0,0};
    float XS[1024],YS[1024];
    int x_n=0,y_n=0;

    //for(CirclePartShape &part :parts)
    {
        //part.sum_of_force={0,0};
        //part.global_pos=pos+part._pos;
        for(PlatformLine *line: core->lines){

            Point p=_next_pos-line->pos;
            Point mz=p.rotate(line->l.mozdavaj);

            Point prv_p=pos-line->pos;
            Point prv_mz=prv_p.rotate(line->l.mozdavaj);

            if((prv_mz.y <-0.00001 && mz.y>0.00001)||
                    (prv_mz.y >0.00001 && mz.y<-0.00001)){
                float mz_x=(prv_mz.x*abs(mz.y)+mz.x*abs(prv_mz.y))/
                        (abs(prv_mz.y)+abs(mz.y));
                if(abs(mz_x)<line->l.h_d){
                    for_remove=true;
                    if(view)
                        view->on_ded();
                    view=nullptr;
                    return;
                }

            }
            if(abs(mz.x)<line->l.h_d && abs(mz.y)<r){
                float f=r-abs(mz.y);
                if(view)
                    view->on_ded();
                view=nullptr;
                return;
            }
        }
    }
    for(auto block:core->blocks)if(block){
        float R=this->r+block->r;
        Point p=block->pos-pos;
        if(p.length()<R){
           for_remove=true;
           if(view)
               view->on_ded();
           view=nullptr;
        }
    }
    for(auto tank_ptr:core->tanks)if(tank_ptr.second && tank_ptr.second->id!=owner_id){
        auto tank=tank_ptr.second;
        float R=r+tank->r;
        Point p=tank->pos-pos;
        float d=p.length();
        if(d<R){
           for_remove=true;
            tank->get_damage(damage);
           if(view)
               view->on_ded();
           view=nullptr;
        }
    }/**/
    float max_d=core->max_d;
    if(pos.length()>max_d){
        for_remove=true;
        if(view){
            view->on_ded();
            view=nullptr;
        }
    }


}





