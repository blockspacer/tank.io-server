#ifndef ROOM_H
#define ROOM_H
struct MainServer;
#include "Engine/BaseRoom.h"
#include <map>
#include "GameCore/GameCore.h"
#include "GameCore/AbstractTankViewHandler.h"
#include "GameCore/BoardObjectEvent.h"

#include <boost/asio.hpp>
#include <unordered_map>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/move/move.hpp>
#include <boost/circular_buffer.hpp>
#include "base/asyncmap.h"
#include <chrono>



using namespace std;

struct ServerViewShadow:public ViewShadow{
    struct ServerViewHandler* master;

    ServerViewShadow(ServerViewHandler *master,
                     const BoardObject *_obj):ViewShadow(_obj),master(master){};
    virtual void on_ded();
};
struct ServerViewHandler:public ViewHandler{
    /*int id_g=0;
    int generate_event_id(){
        return ++id_g;
    }/**/

    map<OBJ_ID,STEP_VALUE> time_og_last_change;
    map<OBJ_ID,map<OBJ_ID,STEP_VALUE> > ackSendedEvent;
    //map<OBJ_ID,OBJ_ID> object_2_event;
    //BoardObjectEvent* last_event=nullptr;
    vector<OBJ_ID> last_step_events;

    struct Room *room;

    ServerViewHandler(GameCore *gm,struct Room *room):room(room){
        game_core=gm;
    }

    void handle_shot(Misle *shot);
    void handle_tank(Tank *tank);
    void handle_block(CircleBlock *block);
    void handle_block(PlatformLine *block);
    void get_array(vector<MyDataBlock> &forSend,TankState16Msg *stats);
    void send_to_user(struct UserPort *up, BoardObjectEvent*);
    void send_to_users(BoardObjectEvent*);
    void triger_change(int object_id);

    void init_client(struct UserPort *up,int last_got_event_id);
};

struct Room:public BaseRoom{
    boost::shared_mutex mutex;
    shared_ptr<GameCore> game_core;
    long last_action=0;
    ServerViewHandler *handler=nullptr;
    struct RoomData{
        ROOM_DATA data;
        int lig_id;
        long long starting_time=0;
        //Room::PlayerPos player_pos[256];
        TIME_VALUE last_update_time;
    };

    int lig_id;
    OBJ_ID db_id=0;
    MainServer *server;

    void save_result();
    void say_to_client_game_closed();
    void set_response_msg(JoinResponse *out,USER_ID us);
    void set_winer(int winer){

        save_result();
    }

    void init_with_room_date(RoomData &r);

    static std::shared_ptr<Room> create(MainServer *ser);

    map<USER_ID,UserPort*> users;
    map<USER_ID,int> last_shot_id;
    TIME_VALUE last_update_time;


    bool action_msg_is_ok(RoomMsg *msg){
        if(data.code!=msg->room.code)
            return false;

        if(users.find(msg->user_id)==users.end())
            return false;

        //player_in__room[users[msg->user_id]]=true;

        return true;
    }
    bool update();
    int passd[3]={0,0,0};
    bool room_is_close=false;

    void update_client(struct UserPort *up,int last_got_event_id);
    void update_clients();
    void event_ack(UserPort *user_id,OBJ_ID event_id,STEP_VALUE t);


    void get_msg(UserPort *up,GameActionMsg *msg);

};


#endif // ROOM_H
