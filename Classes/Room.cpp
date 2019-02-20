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
    auto last=object_2_event[obj_id];
    if(last>0){
        events.erase(last);
    }
    BoardObjectEvent *e=new BoardObjectEvent();
    object_2_event[obj_id] = e->id = generate_event_id();
    e->game_core = this->game_core;
    e->target_object_id = obj_id;
    events[e->id]=e;
    last_step_events.push_back(e);

}
void ServerViewHandler::get_array(BoardObjectEvent*e,TankState16Msg *tank_state){
    int size=sizeof(TankState16Msg);
    tank_state->number_of_states=1;

    char *data=(char*)tank_state;
    data+=tank_state->start_states;


    BoardObjectState *state=(BoardObjectState*)data;
    e->get_data(state);
    size+=state->size_of_data;
    tank_state->size_of_data=size;
}
void ServerViewHandler::send_to_user(UserPort *up, BoardObjectEvent* e){
    char _data[256];
    TankState16Msg *tank_state=new(_data) TankState16Msg();
    get_array(e,tank_state);

    room->server->udp_send(up,tank_state,tank_state->size_of_data);

}
void ServerViewHandler::send_to_users(BoardObjectEvent *e){

    char _data[256];
    TankState16Msg *tank_state=new(_data) TankState16Msg();
    get_array(e,tank_state);

    for(auto t:room->users){
         UserPort *up=t.second;
         room->server->udp_send(up,tank_state,tank_state->size_of_data);
    }/**/


}
void ServerViewHandler::init_client(UserPort *up,int last_got_event_id){



    vector<int> for_remove;

    for(auto t=events.lower_bound(last_got_event_id); t!=events.end(); ++t)if(t->second!=nullptr){
        if(game_core->get_object(t->second->target_object_id)==nullptr){//TODO crash kard t->second ajib bud
            for_remove.push_back(t->second->target_object_id);
            continue;
        }
        auto &set=ackSendedEvent[t->second->id];

        if(set.find(up->client_process_id)!=set.end())
            continue;

        send_to_user(up,t->second);

    }
    for(auto t:for_remove){
        events.erase(t);
    }
}
void Room::update_client(UserPort *up,int lsat_got_event){



    handler->init_client(up,lsat_got_event);
}

void Room::update_clients(){

    for(auto e : handler->last_step_events)
        handler->send_to_users(e);
    handler->last_step_events.clear();

    for(auto user:users){
        update_client(user.second,user.second->last_got_event);
    }
}
void Room::event_ack(UserPort *up,OBJ_ID event_id){
    //up->incom_events.insert(event_id);
    if(handler->events.find(event_id)!=handler->events.end()){
      handler->ackSendedEvent[event_id].insert(up->client_process_id)  ;
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
