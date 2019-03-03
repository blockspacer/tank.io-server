#include "Room.h"
#include "MainServer.h"
#include "GameCore/Tank.h"
#include "GameCore/Misle.h"
#include "GameCore/PlatformLine.h"

void ServerViewShadow::on_ded(){
    master->triger_change(obj->id);
}

void ServerViewHandler::handle_shot(Misle *shot){
    shot->view=new ServerViewShadow(this,shot);
    triger_change(shot->id);
}

void ServerViewHandler::handle_tank(Tank *tank){
    tank->view=new ServerViewShadow(this,tank);
    triger_change(tank->id);
}
void ServerViewHandler::handle_block(CircleBlock *block){
    block->view=new ServerViewShadow(this,block);
    triger_change(block->id);

}
void ServerViewHandler::handle_block(PlatformLine *block){
    block->view=new ServerViewShadow(this,block);
    triger_change(block->id);

}
void ServerViewHandler::triger_change(int obj_id){
    time_og_last_change[obj_id]=game_core->total_time;
    last_step_events.push_back(obj_id);

}
void ServerViewHandler::get_array(vector<MyDataBlock> &forSend,TankState16Msg *tank_state){

    tank_state->size_of_data=sizeof(TankState16Msg);
    size_t n=forSend.size();
    if(n>16)
        n=16;
    tank_state->array.init(tank_state,n);
    for(size_t i=0; i<n; ++i){
        auto &t=forSend[i];
        //MyPointer p;
        MyPointer *pp=tank_state->array.getIndex(i);
        pp->init(tank_state,t.size);
        memcpy(pp->getPtr(),(void*)t.data,t.size);
        cerr<<pp->getPtr<BoardObjectState>()->object_id<<endl;
        cerr<<pp->getPtr<BoardObjectState>()->room_time<<endl;
    }

}

void ServerViewHandler::send_to_user(UserPort *up, BoardObjectEvent* e){
    char _data[256];
    TankState16Msg *tank_state=new(_data) TankState16Msg();

    vector<MyDataBlock> forSend;
    MyDataBlock res=e->get_data();
    forSend.push_back(res);
    get_array(forSend,tank_state);

    room->server->udp_send(up,tank_state,tank_state->size_of_data);

}
void ServerViewHandler::send_to_users(BoardObjectEvent *e){

    char _data[256];
    TankState16Msg *tank_state=new(_data) TankState16Msg();
    vector<MyDataBlock> forSend;
    MyDataBlock res=e->get_data();
    forSend.push_back(res);
    get_array(forSend,tank_state);

    for(auto t:room->users){
         UserPort *up=t.second;
         room->server->udp_send(up,tank_state,tank_state->size_of_data);
    }/**/


}
void ServerViewHandler::init_client(UserPort *up,int last_got_event_id){



    vector<OBJ_ID> for_remove;
    vector<MyDataBlock> forSend;

    auto &set=ackSendedEvent[up->client_process_id];
    for(auto t : game_core->all_objects){
        auto obj=game_core->get_object(t.first);
        if(obj==nullptr || obj->for_remove)
            continue;



        if(set.find(t.first)!=set.end() && set[t.first] >= obj->last_event_time)
            continue;



        MyDataBlock res=game_core->get_object(t.first)->get_data();
        if(res.size>100){
            cerr<<"res.size>100"<<endl;
        }
        forSend.push_back(res);

    }
    for(auto t:for_remove){
        time_og_last_change.erase(t);
    }
    try{
        if(forSend.size()<1)
            return ;
        cerr<<"new tank state "<<forSend.size()<<endl;
        char *_data=new char[4*1024];
        TankState16Msg *tank_state=new(_data) TankState16Msg();
        get_array(forSend,tank_state);
        cerr<<tank_state->array.getArray()[0].getPtr<BoardObjectState>()->room_time<<endl;
        cerr<<"fff "<<sizeof (TankState16Msg)+sizeof(TankState)<<" "<<tank_state->size_of_data<<endl;

        room->server->udp_send(up,tank_state,tank_state->size_of_data);

        cerr<<&(tank_state->array.getArray()[0].getPtr<BoardObjectState>()->room_time)-(int*)tank_state<<endl;
        cerr<<"fff2 "<<sizeof (TankState16Msg)+sizeof(TankState)<<" "<<tank_state->size_of_data<<endl;
    }catch(std::exception& e){
        cerr<<"exception "<<e.what()<<endl;
    }


}
void Room::update_client(UserPort *up,int lsat_got_event){



    handler->init_client(up,lsat_got_event);
}

void Room::update_clients(){

    //for(auto e : handler->last_step_events)
    //    handler->send_to_users(e);
    //handler->last_step_events.clear();

    for(auto user:users){
        update_client(user.second,user.second->last_got_event);
    }
}
void Room::event_ack(UserPort *up,OBJ_ID event_id,STEP_VALUE step){
    //up->incom_events.insert(event_id);
    if(handler->time_og_last_change.find(event_id)!=handler->time_og_last_change.end()){
      if(handler->ackSendedEvent[up->client_process_id][event_id]<step)
          handler->ackSendedEvent[up->client_process_id][event_id]=step;
    }
}

bool Room::update(){
    if(room_is_close)
        return false;
    if(last_action+60*1000<game_core->total_time)
        return false;
    int i=0;
    while(game_core->get_step_with_time(server->total_time)>game_core->total_time){
        ++i;
        game_core->update();
        //send_to_users(e); TODO
    }
    //_show(i);
    return true;


}



void Room::init_with_room_date(RoomData &res){


    data=res.data;
    lig_id=res.lig_id;

    last_update_time=res.last_update_time;
    last_update_time=server->total_time;

}
std::shared_ptr<Room> Room::create(MainServer *server){
    auto room=std::make_shared<Room>();
    room->game_core=std::make_shared<GameCore>();
    room->game_core->view_handler=
            room->handler=
            new ServerViewHandler(room->game_core.get(),room.get());

    room->game_core->total_time=0;
    room->game_core->milisecond_per_step=20;
    room->game_core->start_time=server->total_time;
    room->game_core->server=true;
    room->game_core->init();
    room->server=server;


    return room;
}



void Room::get_msg(UserPort *up,GameActionMsg *mr){
    if(!action_msg_is_ok(mr))
        return;
    last_action=game_core->total_time;
   boost::unique_lock<boost::shared_mutex> lock_qurd(mutex);
    update();
    std::shared_ptr<Tank::Input::State> action=mr->get_action();
    auto player=game_core->get_player(mr->user_id);
    if(player!=nullptr && player->tank){
       STEP_VALUE action_time=action->trigger_step;
       int dt=2;
       if(action_time<game_core->total_time+dt)
           action_time=game_core->total_time+dt;
       int act_time=action_time;
       Tank::Input *inp=player->tank->get_action(action->client_id);
       if(inp!=nullptr){

           char _data[256];
           GameActionMsg *tank_state=new(_data) GameActionMsg();
           tank_state->number_of_states=1;
           char *_array=_data+tank_state->start_states;
           auto inp_State=(Tank::Input::State*)_array;
           inp->get_data(inp_State);

           inp_State->tank_id = player->tank->id;
           tank_state->size_of_data=sizeof(GameActionMsg)+inp_State->size_of_data;

           server->udp_send(up,tank_state,tank_state->size_of_data);/**/
           return;
       }

       if(action->type==Tank::Input::State::Type::MOVMENT){
           auto cs=static_cast<Tank::InputFarman::State*>(action.get());
           inp=player->tank->add_action(act_time,cs->farman,action->client_id);
       }else
           inp=player->tank->add_shot_action(act_time,action->client_id);

       {
           char _data[256];
           GameActionMsg *tank_state=new(_data) GameActionMsg();
           tank_state->number_of_states=1;
           char *_array=_data+tank_state->start_states;
           auto inp_State=(Tank::Input::State*)_array;
           inp->get_data(inp_State);

           inp_State->tank_id = player->tank->id;

           tank_state->size_of_data=sizeof(GameActionMsg)+inp_State->size_of_data;
           cerr<<"send ack of action"<<endl;
           tank_state->id=0;

           server->udp_send(up,tank_state,tank_state->size_of_data);/**/

       }
       for(auto i:users){
           UserPort *up2=i.second;

           if(up2==up)
               continue;
           char _data[256];
           GameActionMsg *tank_state=new(_data) GameActionMsg();
           tank_state->number_of_states=1;
           char *_array=_data+tank_state->start_states;
           auto inp_State=(Tank::Input::State*)_array;
           inp->get_data(inp_State);
           inp_State->client_id=action->client_id;
           inp_State->tank_id = player->tank->id;

           tank_state->size_of_data=sizeof(GameActionMsg)+inp_State->size_of_data;

           server->udp_send(up2,tank_state,tank_state->size_of_data);/**/


       }

    }

}
