#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
//#include <boost/asio.hpp>
#include <set>
#include <hash_map>
#include <map>

#include <unordered_map>
#include "Classes/MainServer.h"
#include "game_message/Messages.h"
#include "base_message/BaseMessage.h"
#include "GameCore/Tank.h"
#include "debuge_tools/DebugeTools.h"
using boost::asio::ip::udp;
using namespace std;

void MainServer::after_register(UserPort *s,USER_ID user_id){
    update_client_profile(user_id);
}

void MainServer::new_login(USER_ID user_id,UserPort *user_port){
    //return ;
    UserPort *ls=users_with_id(user_id);
    if(ls!=nullptr){
        user_port->on_room=ls->on_room;
        user_port->last_msg_id=ls->last_msg_id;
    }
    /*for(auto tt :PositionData::posins){
        PositionData &pd=tt.second;
        GameConfigResponse *t=new GameConfigResponse();
        t->gem=pd.price;
        t->config_id=pd.id;
        for(int i=0; i<5; ++i)
            t->config.palyer_pos[i]=ZIP_Point::fcreate(pd.postions.players_pos[i].x,pd.postions.players_pos[i].y);
        t->active=pd.active;
        t->have=false;
        udp_send_and_push_out_box_to_handle_ACK(user_port,t,sizeof(GameConfigResponse));
    }/**/
}



void ChangeNameJob::init(UserPort *user_port){
    this->ses=user_port;
    this->server=server;
}

void ChangeNameJob::do_job(){
    ChangeNameResponse r=static_cast<MainServer*>(server)->change_display_name(ses->user_data);

    if(r.done)
        ses->tcp_send(&r,sizeof(ChangeNameResponse));
    static_cast<MainServer*>(server)->update_client_profile(ses->user_data->id);

}

void SaveUserData::init(const USER_ID user_id){

    this->user_id=user_id;

}

void SaveUserData::do_job(){
    server->save_user_data(user_id);
    //ses->tcp_send(&r,sizeof(ChangeNameResponse));
}



void SaveUseMony::init(const USER_ID user_id,int mony,const USE_DISCRIP dis){
    this->mony=mony;
    strcpy(this->dis,dis);
    this->user_id=user_id;
}

void SaveUseMony::do_job(){
    static_cast<MainServer*>(server)->save_use_mony(user_id,mony,dis);
}


void LoadExtraData::init(USER_ID user_id){
    this->server=server;
    this->user_id=user_id;
}
void LoadExtraData::do_job(){
    server->db->load_extra_if_not_exist(user_id);
    static_cast<MainServer*>(server)->update_client_game_data(user_id);
}

void SaveTeamLevel::init(USER_ID user_id, int team_id){
    this->user_id=user_id;
    this->team_id=team_id;

}
void SaveTeamLevel::do_job(){
    server->db->save_team_levl(user_id,team_id);
}


void SaveConfigs::init(USER_ID user_id, int postion_id){
    this->server=server;
    this->user_id=user_id;
    this->postion_id=postion_id;

}
void SaveConfigs::do_job(){
    server->db->save_postions(user_id,postion_id);
}



void SaveRoom::init(Room *room){
    this->server=server;
    this->room=room;


}
void SaveRoom::do_job(){
    server->db->save_room(room);
}
ChangeNameResponse MainServer::change_display_name(UserData *ud){
    ChangeNameResponse r;
    r.done=db->change_display_name(ud);
    strcpy(r.display_name,ud->display_name);
    return r;
}
void MainServer::save_user_data(USER_ID user_id){
    db->save_user_data(user_id);
}
void MainServer::save_use_mony(USER_ID uid, int mony, string dis){
    db->save_use_mony(uid,mony,dis);
}

void MainServer::buge_repoart(UserPort *s, BugCheckPoint *msg){
    db->bug_check_point(s->user_data->id);
}


void MainServer::profile_request(UserPort *s,RequestProfile *mr){
    if(mr->user_id==s->user_data->id){
        update_client_profile(mr->user_id);
        update_client_game_data(mr->user_id);
        return ;
    }
    ResponseProfile *first_player_profile=get_user_profile_msg(mr->target_user_id);
    udp_send_and_push_out_box_to_handle_ACK(users_with_id(mr->user_id) ,first_player_profile,sizeof(ResponseProfile));


}

void MainServer::lig_win(USER_ID user_id, int lig_id,int coin_win){
  auto data=get_user_data_by_id(user_id);
  data->exp+=lig_exp[lig_id];

  int gem=coin_win/1000;;
  if(gem>0){
      coin_win-=gem*30;
  }
  data->coin+=coin_win;


  data->win_number+=1;


  int b_level=data->level;
  data->level=0;
  for(int i=0; lvl_exp[i]<data->exp;++i)
      data->level=i;
  int added_gem=0;
  for(int i=b_level; i<data->level;++i){
    added_gem+=lvl_gem[i];
  }
  add_achivment(user_id,2*lig_id);
  data->total_coin_winer+=coin_win;

  if(added_gem!=0 && users_with_id(user_id)!=nullptr){
      GemWin *g=new GemWin();
      g->win_type=GemWin::WinType::LEVEL_UP;
      g->gems=added_gem;
      g->parms1=b_level;
      g->parms2=data->level;
      users_with_id(user_id)->events.push_back(g);

  }




}

void MainServer::add_achivment(USER_ID user_id, int achivment){
  auto data=get_user_data_by_id(user_id);
}

void MainServer::game_request(UserPort *s,GameRequest *msg){



}
void MainServer::game_request_response(UserPort *s, GameRequestResponse *msg){

}


Room* MainServer::create_room(int lig_id){

    shared_ptr<Room> room=Room::create(this);
    room->data.code=get_random_long_long();
    room->data.id=game_count++;
    room->lig_id=lig_id;

    this->rooms[room->data.id]=room;
    rooms_lig[room->lig_id].push_back(room->data.id);
    return room.get();

}
bool MainServer::check_user_in_room(UserPort *s,UserPort *s2){
    return false;
}
void MainServer::update_room_client(Room *room,UserPort *up){

    int i=0;
    /*for(auto t:room->handler){
        if(t!=nullptr){
            TankState16Msg *tank_state=new TankState16Msg();
            TankState &s=tank_state->states;
            t->get_data(&s);
            udp_send(up,tank_state,sizeof(TankState16Msg));
        }
    }/**/


}

void MainServer::run_rooms_step(){
    vector<OBJ_ID > forRemove;
    for(const std::pair<OBJ_ID , std::shared_ptr<Room> > &room:rooms)if(room.second){
        boost::unique_lock<boost::shared_mutex> lock_qurd(room.second->mutex);
        bool ph=room.second->update();
        room.second->update_clients();
        if(!ph)
            forRemove.push_back(room.first);
    }
    for(auto t :forRemove){
        rooms.erase(t);
        //rooms_lig[rooms[t]->lig_id].erase(t);
    }
}
void MainServer::run_rooms_while(){
    while(true){

        run_rooms_step();
        int dt=1000/25;
        boost::this_thread::sleep(boost::posix_time::milliseconds(dt));
    }
}
void MainServer::run_rooms(){
    boost::thread_group threads;
    std::thread *t2=new std::thread([this](){

       this->run_rooms_while();
    });
    /*boost::thread t{[this](){
            this->run_rooms_while();
         }};
    t.join();/**/
}
void MainServer::update_client(Room *room){
    for(auto t:room->users){
        UserPort *up=users_with_id(t.first);
        update_room_client(room,up);
    }

}

void MainServer::match_request(UserPort *s,JoinRequest *mr){

    DEBUGE(cerr<<"math request on lig"<<mr->lig_id<<endl);
    DEBUGE(cerr<<"lig requster size before"<<ligs[mr->lig_id].size()<<endl);

    if(check_user_in_room(s,s))
        return ;
    auto &lrooms=rooms_lig[mr->lig_id];
    Room *room=nullptr;
    if(true)for(OBJ_ID roomId : lrooms){
        if(rooms.find(roomId)!=rooms.end() && rooms[roomId]!=nullptr)
            room=rooms[roomId].get();
    }

    if(room==nullptr)
        room=create_room(mr->lig_id);

    room->game_core->add_player_with_tank(mr->user_id);
    room->users[mr->user_id]=s;
    JoinResponse *jr=new JoinResponse(s->generate_msg_id(),mr->lig_id,room->data);
    jr->lig_id=mr->lig_id;
    jr->room=room->data;
    jr->game_start_time=room->game_core->start_time;
    jr->mili_second_per_step=20;

    udp_send_and_push_out_box_to_handle_ACK(s,jr,sizeof(JoinResponse));
    update_client(room);

}
ResponseProfile *MainServer::get_user_profile_msg(USER_ID user_id){
    UserData *user_data=get_user_data_by_id(user_id);
    if(user_data==nullptr)
        return 0;


    if(user_data->free_coin_time<total_time && user_data->free_coin_time>0){
        user_data->free_coin_time=-1;
        user_data->coin+=300;
    }

    if(user_data->coin<100 && user_data->free_coin_time<=1000){
        EmptyCashCoin *c=new EmptyCashCoin();
        c->coin=300;
        c->minute=2*60;
        user_data->free_coin_time=total_time+c->minute*60*1000;
    }
    if(user_data->coin < 200)
        user_data->coin=200;

    ResponseProfile *p2=new ResponseProfile();


    p2->target_user_id=user_id;
    strcpy(p2->display_name,user_data->display_name);
    p2->level=user_data->level;

    p2->coin=user_data->coin;


    p2->win_number=user_data->win_number;
    p2->loss_number=user_data->lose_number;
    p2->avatar_id=user_data->avatar_id;
    p2->exp=user_data->exp;
    p2->id=0;

    return p2;

}

void MainServer::update_client_game_data(USER_ID user_id){
    UserPort *s=users_with_id(user_id);
    if(s->user_data->extra_data!=nullptr){
        UserGameData *user_game_data=new UserGameData();
        for(int i=0; i<64;++i)
            user_game_data->teams.a[i]=s->user_data->extra_data->team_level[i];
        long long b=1;
        user_game_data->configs.a[0]=0;
        for(int i=0; i<63; ++i,b<<=1){
            bool ph=s->user_data->extra_data->postions[i];
            if(ph)
                user_game_data->configs.a[0]|=b;
        }
        udp_send_and_push_out_box_to_handle_ACK(s,user_game_data,sizeof(UserGameData));
    }
}

void MainServer::update_client_profile(USER_ID user_id){



    ResponseProfile *p= get_user_profile_msg(user_id);
    UserPort *up=users_with_id(user_id);
    if(up==nullptr || p==nullptr)
        return;
    udp_send_and_push_out_box_to_handle_ACK(up,p,sizeof(ResponseProfile));
    for(int i=0; i<up->events.size();++i){
        udp_send_and_push_out_box_to_handle_ACK(up,up->events[i],sizeof(GemWin));
    }
    up->events.clear();
    auto ud=get_user_data_by_id(user_id);

}

void MainServer::server_data_request(UserPort* s,ServerDataRequest *msg){
    return ;
    if(msg->last_data_version >=11)
        return;
    ServerLongMsg *response=new ServerLongMsg();//db->get_server_data(msg->last_data_version);
    if(response==nullptr)
        return;
    strcpy(response->a,"لطفا بازی را از کافه بازار "
                        "\n"
                       "آپدیت کنید");
    response->last_app_version=11;
    response->data_version=10;
    udp_send_and_push_out_box_to_handle_ACK(s,response,sizeof(ServerLongMsg));
}


void MainServer::hundred_state_request(UserPort *s, HundredStateRequest *msg){
    if(s->last_hundred_request+1000>total_time)
        return;
    s->last_hundred_request=total_time;
    auto *r=new HundredStateResponse();
    r->answer_of_msg_id=msg->id;
    int n=0;
    if(msg->n > 50 )
        msg->n=50;
    for(int i=0; i<msg->n;++i){
        UserData *ud=db->get_user_data_by_id(msg->a[i]);
        if(ud==nullptr)
            continue;
        r->profiles[i].id=ud->id;
        r->profiles[i].avatar_id=ud->avatar_id;
        r->profiles[i].level=ud->level;
        strcpy(r->profiles[i].name,ud->display_name);
        ++n;
    }
    r->n=n;
    udp_send_and_push_out_box_to_handle_ACK(s,r,sizeof(HundredStateResponse));


}

void MainServer::top_hundred_request(UserPort *s, TopHundredRequest *msg){
    if(s->last_top_hundred_request+1000>total_time)
        return;
    s->last_top_hundred_request=total_time;
    {
        USER_ID top_list[100];
        int rank;
        int count=db->get_top_hundred(s->user_data,msg->shift,msg->count,rank,top_list);

        //msg->rank=rank;

        TopHundredResponse *r=new TopHundredResponse();
        r->count=count;
        r->shift=msg->shift;
        for(int i=0; i<count;++i){
            UserData *ud=db->get_user_data_by_id(top_list[i]);
            r->profiles[i]=top_list[i];

       }
        udp_send_and_push_out_box_to_handle_ACK(s,r,sizeof(TopHundredResponse));
    }

}

void MainServer::update_user_db(USER_ID user_id ){
    std::shared_ptr<SaveUserData>  lj=std::make_shared<SaveUserData>(this);
    lj->init(user_id);
    db_jobs.push(lj);
}

void MainServer::room_msg(UserPort *s, RoomMsg *mr){

}
void MainServer::first_udp_ping(UserPort *u){
    check_user_in_room(u,u);
    std::shared_ptr<LoadExtraData>  lj=std::make_shared<LoadExtraData>(this);
    lj->init(u->user_data->id);
    db_jobs.push(lj);


}




void MainServer::game_room_msg(UserPort *s,GameActionMsg *mr){
    OBJ_ID room_id=mr->room.id;
    auto room=this->rooms[room_id];
    if(room==nullptr)
         return ;
    room->get_msg(s,mr);




}

void MainServer::close_game(UserPort *s, CloseGame *mr){
    OBJ_ID room_id=mr->room.id;



}




void MainServer::change_dispaly_name(UserPort *s,ChangeNameRequest *mr){

    if(s->user_data->name_changed>2)
        return ;
    s->user_data->name_changed+=1;
    //ip_db_msg_count[this->sender_endpoint_.address()]++;
    strcpy(s->user_data->display_name,mr->display_name);
    std::shared_ptr<ChangeNameJob>  lj=std::make_shared<ChangeNameJob>(this);


    lj->init(s);
    db_jobs.push(lj);



}

void MainServer::room_event_ack(UserPort *s, RoomEventAck *mr){
    OBJ_ID room_id=mr->room.id;
    Room *room=this->rooms[room_id].get();
    if(room==nullptr)
        return;
    room->event_ack(s,mr->obj_id,mr->step);

}


void MainServer::use_mony(UserPort *s,UseMony *mr){

}

#define CHECK_CALL(type,msg,func)  if(((type*)msg)->check()) func(s,(type*)msg);
#define CHECK_CALL3(type,msg,func,byte_recive)  if(((type*)msg)->check(byte_recive)) func(s,(type*)msg);

void MainServer::recive_msg(UserPort *s, BaseMessage *_msg,size_t byte_recive){
      MsgHeader *msg=(MsgHeader*)_msg;



     if(msg->type==MsgHeader::Type::MATCH_REQUEST)
         CHECK_CALL(JoinRequest,msg,match_request);

     if(msg->type==MsgHeader::Type::PROFILE_REQUEST)
         CHECK_CALL(RequestProfile,msg,profile_request);


     if(msg->type==MsgHeader::Type::BUG_CHECK_POINT)
         CHECK_CALL(BugCheckPoint,msg,buge_repoart);

     if(msg->type==MsgHeader::Type::GAMEACTION_CLIENT_SEND)
         CHECK_CALL3(GameActionMsg,msg,game_room_msg,byte_recive);

     if(msg->type==MsgHeader::Type::CLOSE_GAME)
         CHECK_CALL(CloseGame,msg,close_game);






     if(msg->type==MsgHeader::Type::CHANGE_DISPLAY_NAME_REQUEST)
         CHECK_CALL(ChangeNameRequest,msg,change_dispaly_name);


     if(msg->type==MsgHeader::Type::USE_MONY)
         CHECK_CALL(UseMony,msg,use_mony);

     if(msg->type==MsgHeader::Type::GAME_REQUEST)
         CHECK_CALL(GameRequest,msg,game_request);

     if(msg->type==MsgHeader::Type::GAME_REQUEST_RESPONSE)
         CHECK_CALL(GameRequestResponse,msg,game_request_response);


     if(msg->type==MsgHeader::Type::SERVER_DATA_CHECK_REQUEST)
         CHECK_CALL(ServerDataRequest,msg,server_data_request);

     if(msg->type==MsgHeader::Type::TOP_HUNDERD_SCORE_REQUEST)
         CHECK_CALL(TopHundredRequest,msg,top_hundred_request);

     if(msg->type==MsgHeader::Type::HUNDERD_STATE_REQUEST)
         CHECK_CALL(HundredStateRequest,msg,hundred_state_request);



     if(msg->type==MsgHeader::Type::ROOM_EVENT_ACK)
         CHECK_CALL(RoomEventAck,msg,room_event_ack);

     if(msg->type==MsgHeader::Type::ROOM_STATE_REQUST){

              RoomStateRequest *mr=(RoomStateRequest*)msg;
              OBJ_ID room_id=mr->room.id;

              Room *room=this->rooms[room_id].get();

              if(room && room->action_msg_is_ok(mr)){
                    s->last_got_event=mr->last_got_event;
                    room->update_client(s,mr->last_got_event);

              }
          }
}
void MainServer::change_user_data(USER_ID user_id){

    std::shared_ptr<SaveUserData> lj=std::make_shared<SaveUserData>(this);
    lj->init(user_id);
    db_jobs.push(lj);

    update_client_profile(user_id);
}

void MainServer::recive_tcp_msg(UserPort *s, BaseMessage *_msg){
    RequestProfile *msg=(RequestProfile*)_msg;
    if(msg->type==MsgHeader::Type::PROFILE_REQUEST){
        if(msg->target_user_id==s->user_data->id)
            update_client_profile(s->user_data->id);
        else
            profile_request(s,msg);
    }
}
void MainServer::reload_rooms(){
    BaseServer::reload_rooms();

    MyHashMap *m=   SharedRoomMemory::get_map();
    if(m==nullptr)
        return ;
    game_count=10;
    vector<int> remov_id;
    for(MyHashMap::iterator i=m->begin(); i!=m->end();++i){
        Room::RoomData &rd=i->second;
        long long lt=total_time-rd.starting_time-rd.last_update_time;
        if(120*1000<lt){
            remov_id.push_back(i->first);
            continue;
        }
        if(rd.starting_time+60*60*1000<total_time){
            remov_id.push_back(i->first);
            continue;
        }
        auto r=Room::create(this);
        r->init_with_room_date(rd);
        rooms[r->data.id]=r;
        if(game_count<r->data.id)
            game_count=r->data.id;
    }
    for(int i=0; i<remov_id.size();++i)
        SharedRoomMemory::remove(remov_id[i]);
    ++game_count;

}
