#include "BoardObjectEvent.h"
#include "GameCore.h"
#include "Tank.h"
#include "Misle.h"
#include "PlatformLine.h"
void BoardObjectEvent::get_data(BoardObjectState *s){
    //size=sizeof(*this);
    //memcpy(data,*this,size);
    //*(int*)data=1;
    if(game_core->get_object(target_object_id)==nullptr || game_core->get_object(target_object_id)->for_remove){
        s->event_type=BoardObjectState::REMOVE;
        s->object_id=target_object_id;
        return ;
    }
    s->event_type=BoardObjectState::ADD;
    s->event_id=id;
    BoardObject *tank=game_core->get_object(target_object_id);
    tank->get_data(s);


}
void BoardObjectEvent::run_game_core(BoardObjectState *s){
    if(!game_core)
        return;
    target_object_id=s->object_id;
    if(game_core->get_object(target_object_id)==nullptr && s->event_type!=BoardObjectState::EventType::REMOVE){
        //TODO create remove event
        if(s->object_type==BoardObjectState::ObjectType::CircleBlock){
            auto c=game_core->create_block();
            c->set_data(s);
            game_core->init_block(c);
        }else if(s->object_type==BoardObjectState::ObjectType::LineBlock){
            auto c=game_core->create_platform_line({0,0},{0,0});
            c->set_data(s);
            game_core->init_platform_line(c);
        }else if(s->object_type==BoardObjectState::ObjectType::MISLE){
            auto *m=game_core->create_shot();
            m->set_data(s);
            game_core->init_shot(m);
            if(s->room_time<game_core->total_time){

            }

        }else if(s->object_type==BoardObjectState::ObjectType::TANK){
            auto cs=static_cast<TankState*>(s);
            auto *m=game_core->create_tank();
            m->set_data(s);
            game_core->init_tank(m);
            if(game_core->get_player(cs->user_id)==nullptr){
                Player *p=new Player();
                p->tank=m;
                game_core->players[cs->user_id]=p;
            }

        }
    }
    BoardObject *tank=game_core->get_object(s->object_id);
    //PostionConfig p=tank->getPosConfig();
    if(tank)
        tank->set_data(s);

    LiveBoardObject *tank2=dynamic_cast<LiveBoardObject*>(tank);
    int X=0;
    if(tank2!=nullptr && s->room_time<X+game_core->total_time){
        int t=game_core->total_time - s->room_time+X;
        if(tank2->history.find(s->room_time)!=tank2->history.end()){
           PostionConfig pc=tank2->history[s->room_time];

        }
        for(int i=0; i<t; ++i)
            tank2->update();
        Point p2=tank->pos;
        //p2-=p.pos;
        CCLOG("t= %d ",t);
        CCLOG("   %f %f ",p2.x,p2.y);
    }
}

