#include "Misle.h"
#include "PlatformLine.h"
#include "Tank.h"
#include "AbstractTankViewHandler.h"
#include "GameCore.h"



void Misle::get_data(BoardObjectState *s)const{
    LiveBoardObject::get_data(s);
    s->object_type=BoardObjectState::ObjectType::MISLE;
    s->owner_id=owner_id;
}
MyDataBlock Misle::get_data()const{
    auto data=new LiveBoardObjectState();
    MyDataBlock res;
    get_data(data);
    res.data = (char*)data;
    res.size = sizeof(LiveBoardObjectState);
    return res;
}
void Misle::set_data(const BoardObjectState *state){
    if(last_update_from_server_time>state->room_time)
        return;
    LiveBoardObject::set_data(state);
    //speed=state->speed;
    owner_id=state->owner_id;
}

void Misle::update(){
    LiveBoardObject::update();
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
            Point mz={p.x*line->mozdavaj.x-p.y*line->mozdavaj.y , p.x*line->mozdavaj.y+p.y*line->mozdavaj.x};

            Point prv_p=pos-line->pos;
            Point prv_mz={prv_p.x*line->mozdavaj.x-prv_p.y*line->mozdavaj.y ,
                          prv_p.x*line->mozdavaj.y+prv_p.y*line->mozdavaj.x};
            if((prv_mz.y <-0.00001 && mz.y>0.00001)||
                    (prv_mz.y >0.00001 && mz.y<-0.00001)){
                float mz_x=(prv_mz.x*abs(mz.y)+mz.x*abs(prv_mz.y))/
                        (abs(prv_mz.y)+abs(mz.y));
                if(abs(mz_x)<line->h_d){
                    for_remove=true;
                    if(view)
                        view->on_ded();
                    view=nullptr;
                    return;
                }

            }
            if(abs(mz.x)<line->h_d && abs(mz.y)<r){
                float f=r-abs(mz.y);
                if(mz.y<0)
                    f*=-1;
                float upc=line->amud.dot(v);
                float emc=line->direction.dot(v);
                force = -line->amud*upc;
                force+= line->direction*emc;
                //part.sum_of_force+=force;
                //force.normalize();
                //v=force*speed; reflect object
                //if(view_handler!=nullptr)
                //    view_handler->show_cross_with_block({0,0},0);
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
            tank->get_damage(10);
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





